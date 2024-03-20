
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

    ; r9 (needle_len) <= rdx (count)

    lea rax, [r9 + 32 - 1]
    cmp rdx, rax
    ja large_vec

    ; Check if haystack (rcx) or haystack + needle_len - 1 (rcx + r9 - 1) are crossing page boundary.
    lea rax, [rcx + r9 - 1]
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    ja cross_page

    ; rdx (count) <= 32

    ; +1 cycle: using vmovdqu with vpcmpeqb
    ; vmovdqu ymm3, YMMWORD PTR [...]
    ; vmovdqu ymm4, YMMWORD PTR [...]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + r9 - 1]

    sub rdx, r9

    mov eax, -1
    bzhi r9d, eax, r9d ; r9d = (1 << r9d) - 1. r9d - needle length

    ; Merge two resuls into single one
    vpand ymm5, ymm3, ymm4

    vmovdqu ymm2, YMMWORD PTR [r8]
    vpmovmskb eax, ymm5
loop_less_vec:
    xor r10d, r10d ; prevent false dependency for tzcnt
    tzcnt r10d, eax
    btr eax, r10d ; set eax bit at r10d position to zero
    cmp r10, rdx
    ja return_zero

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
    ; mov BYTE PTR [0], 1

    mov rax, rcx
    sub rax, r9
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rax + rdx - 32 + 1]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + rdx - 32]

    add rcx, r9

    vmovdqu ymm2, YMMWORD PTR [r8 + r9 - 32]

    vpand ymm5, ymm3, ymm4
    sub rdx, r9
    not rdx

    mov r8, r9
    neg r8
    mov eax, -1
    shlx r8d, eax, r8d

    vpmovmskb eax, ymm5
    shrx eax, eax, edx
cross_page_loop_less_vec:
    test eax, eax
    jz return_zero

    xor r10d, r10d ; prevent false dependency for tzcnt
    tzcnt r10d, eax
    btr eax, r10d ; set eax bit at r10d position to zero

    vpcmpeqb ymm0, ymm2, YMMWORD PTR [rcx + r10 - 32]
    vpmovmskb r11d, ymm0

    andn r11d, r11d, r8d
    jnz cross_page_loop_less_vec

    lea rax, [rcx + r10] ; return match position
    sub rax, r9
    vzeroupper
    ret

    align 16
large_vec:
    include strfind_str/vec_needle.asm

betterstring_strfind_str_avx2 ENDP

END
