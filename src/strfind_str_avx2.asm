
PAGE_SIZE equ 1 SHL 12 ; 4096

.code

; const char* haystack (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to needle
; size_t      needle_len (r9) - lenght of the needle
;
; Finds the first occurrence of the substring [needle, needle + needle_len) in the string [haystack, haystack + count).
;
; Implemented algorithm (without page boundary check & out of range reads):
; if (needle_len > count) { return nullptr; }
; if (needle_len == 0) { return haystack; }
; 
; __m256i first = _mm256_set1_epi8(needle[0])
; __m256i last = _mm256_set1_epi8(needle[needle_len - 1])
; for (size_t i = 0; i < count; i += 32) {
;     __m256i block_first = _mm256_loadu_si256((__m256i*)(haystack + i));
;     __m256i block_last = _mm256_loadu_si256((__m256i*)(haystack + i + needle_len - 1));
;     __m256i eq_first = _mm256_cmpeq_epi8(first, block_first);
;     __m256i eq_last = _mm256_cmpeq_epi8(last, block_last);
;     __m256i first_and_last = _mm256_and_si256(eq_first, eq_last);
;     uint32_t mask = _mm256_movemask_epi8(first_and_last);
;     while (mask != 0) {
;         uint32_t position = tzcnt(mask);
;         uint32_t j = 0;
;         while (true) {
;             __m256i haystack_block = _mm256_loadu_si256((__m256i*)(haystack + i + position));
;             __m256i needle_block = _mm256_loadu_si256((__m256i*)needle);
;             __m256i match_eq = _mm256_cmpeq_epi8(haystack_block, needle_block);
;             uint32_t match = _mm256_movemask_epi8(match_eq);
;             if (match != 0xFFFFFFFF) { break; }
;             j += 32;
;             if (j >= needle_len) { return haystack + i + position; }
;         }
;         btr(mask, position);
;     }
; }
; return nullptr;
;

betterstring_strfind_str_avx2 PROC

    cmp r9, rdx ; check if needle length is greater than haystack size.
    ja return_zero_no_vzeroupper

    test r9, r9 ; check if needle length is zero
    jz return_haystack_no_vzeroupper

    ; Fill ymm0 with first character of the needle
    vpbroadcastb ymm0, BYTE PTR [r8]
    ; Fill ymm1 with last character of the needle
    vpbroadcastb ymm1, BYTE PTR [r8 + r9 - 1]

    cmp rdx, 32
    ja large_vec

    ; Check if haystack (rcx) or haystack + needle_len - 1 (rcx + r9 - 1) are crossing page boundary.
    lea rax, [rcx + r9 - 1]
    or rax, rcx
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    ja cross_page

    ; +1 cycle: using vmovdqu with vpcmpeqb
    ; vmovdqu ymm3, YMMWORD PTR [...]
    ; vmovdqu ymm4, YMMWORD PTR [...]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + r9 - 1]

    mov eax, -1
    bzhi r9d, eax, r9d ; r9d = (1 << r9d) - 1. r9d - needle length
    vmovdqu ymm2, YMMWORD PTR [r8]
    dec edx ; decrement for loop

    ; Waits until ymm2 and ymm3 are ready
    ; Merge two resuls into single one
    vpand ymm5, ymm3, ymm4

    vpmovmskb eax, ymm5
    ; Don't need to clear bits that are outside the string, they will be handled in the loop.
    ; It's saves one cycle (doing parallel cmp in loop vs bzhi).
loop_less_vec:
    xor r10d, r10d ; prevent false dependency for tzcnt
    tzcnt r10d, eax
    btr eax, r10d ; set eax bit at r10d position to zero
    cmp r10d, edx
    jae return_zero

    vpcmpeqb ymm0, ymm2, YMMWORD PTR [rcx + r10]
    vpmovmskb r11d, ymm0

    andn r11d, r11d, r9d ; ~r11d & ((1 << r9d) - 1)
    jnz loop_less_vec ; check if r11d is NOT zero

    lea rax, [rcx + r10] ; return match position
    vzeroupper
    ret

    align 16
return_zero:
    xor rax, rax ; return nullptr
    vzeroupper
    ret

    align 16
return_zero_no_vzeroupper:
    xor rax, rax ; return nullptr
    ret

    align 16
return_haystack_no_vzeroupper:
    mov rax, rcx ; rcx - haystack pointer
    ret

cross_page:
    int 3 ; unimplemented

    align 16
large_vec:
    int 3 ; not fully implemented

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1]
    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4
    test eax, eax

betterstring_strfind_str_avx2 ENDP

END
