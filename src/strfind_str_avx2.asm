
PAGE_SIZE equ 1 SHL 12 ; 4096

.code

; const char* haystack (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to needle
; size_t      needle_len (r9) - lenght of the needle
;
; Finds the first occurrence of the substring [needle, needle + needle_len) in the string [haystack, haystack + count).


MM_SHUFFLE MACRO d:REQ, c:REQ, b:REQ, a:REQ
    EXITM <(d SHL 6) OR (c SHL 4) OR (b SHL 2) OR (a)>
ENDM

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

    lea rax, [rcx + rdx - 1]
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

    ; Merge two resuls into single one
    vpand ymm5, ymm3, ymm4

    mov rax, r8
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    ja needle_cross_page_left
    vmovdqu ymm2, YMMWORD PTR [r8]
needle_cross_page_left_continue:
    mov eax, -1
    bzhi r9d, eax, r9d ; r9d = (1 << r9d) - 1. r9d - needle length

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

; xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx - YMM word
; aaaaaaa - needle
; bbbbbbbbbbbbbbbbbbbb - haystack
; ########### - uninitialized page
; ??????????? - garbage data
;                                   | after haystack begins next page which contents may be uninitialized
;                    ????????????bbbbbbbbbbbbbbbbbbbb###########
;                                |            | needle may be located here (haystack + count - needle_len)
;                                aaaaaaa      aaaaaaa###########
;                    | last available YMM word (haystack + count - 32)
;                    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;       | first YMM word (haystack + needle_len - 32)
;       xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                                | match last character in needle (haystack + count - 32)
;                    ????????????AAAAAAAAAAAAAAAAAAAA
;                          | match first character in needle (haystack + count - 32 - needle + 1)
;                          | including first character of last possible needle location
;              ????????????AAAAAAAAAAAAAAAAAAAA
;
cross_page:
    ; mov BYTE PTR [0], 1

    mov rax, rcx
    sub rax, r9
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rax + rdx - 32 + 1]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + rdx - 32]

    add rcx, r9

    lea rax, [r8 + r9 - 32]
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    ja needle_cross_page_right

    vmovdqu ymm2, YMMWORD PTR [r8 + r9 - 32]
needle_cross_page_right_continue:

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
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1]

    lea rax, [rcx + rdx]
    sub rax, r9
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    jae cross_page_vec

    include strfind_str/vec_needle.asm

    align 16
cross_page_vec:
    include strfind_str/cross_page_vec_needle.asm

    align 16
needle_cross_page_left:
    sub rsp, 64
    vmovdqu ymm2, YMMWORD PTR [r8 + r9 - 32]
    vmovdqu YMMWORD PTR [rsp + r9], ymm2
    vmovdqu ymm2, YMMWORD PTR [rsp + 32]
    add rsp, 64
    jmp needle_cross_page_left_continue

    align 16
needle_cross_page_right:
    sub rsp, 64
    vmovdqu ymm2, YMMWORD PTR [r8]
    vmovdqu YMMWORD PTR [rsp + 32], ymm2
    vmovdqu ymm2, YMMWORD PTR [rsp + r9]
    add rsp, 64
    jmp needle_cross_page_right_continue

betterstring_strfind_str_avx2 ENDP

END
