
; // Copyright 2024.
; // Distributed under the Boost Software License, Version 1.0.
; // (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

PAGE_SIZE equ 1 SHL 12 ; 4096

; const char* string (rcx) - pointer to string in which to count
; size_t      count (rdx) - lenght of string
; char        character (r8b) - character to count
; returns: size_t (rax) - number of characters counted
;
; Note: this function uses AVX2, BMI2, POPCNT processor extensions

; https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/x86_64/multiarch/memchr-avx2.S;h=9a10824f6496abf22a3a05357af58c32719a7f04;hb=HEAD`

; Use COFF's feature grouped sections to define text section with 64 byte alignment
_TEXT$align64 SEGMENT ALIGN(64)
    align 64
betterstring_strcount_char_avx2 PROC

    vmovd xmm0, r8d
    vpbroadcastb ymm0, xmm0

    mov eax, ecx
    and eax, PAGE_SIZE-1
    cmp eax, PAGE_SIZE-32
    ja cross_page_boundary

    mov r8d, -1
    bzhi r9d, r8d, edx
    cmp rdx, 32
    cmovae r9d, r8d

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb eax, ymm1
    and eax, r9d
    popcnt eax, eax

    sub rdx, 32
    jg large_string

return_vzeroupper:
    vzeroupper
    ret

    align 16
first_vec_x4:
    bzhi r8d, r8d, edx
    shrx r8d, r8d, r9d
    popcnt r8d, r8d
    add rax, r8
    sub rdx, 32
    jle return_vzeroupper

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [(rcx+1) + 32*1]
    vpmovmskb r9d, ymm2
    bzhi r9d, r9d, edx
    popcnt r9d, r9d
    add rax, r9
    sub rdx, 32
    jle return_vzeroupper

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [(rcx+1) + 32*2]
    vpmovmskb r10d, ymm3
    bzhi r10d, r10d, edx
    popcnt r10d, r10d
    add rax, r10
    sub rdx, 32
    jle return_vzeroupper

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [(rcx+1) + 32*3]
    vpmovmskb r11d, ymm4
    bzhi r11d, r11d, edx
    popcnt r11d, r11d
    add rax, r11
    sub rdx, 32

    vzeroupper
    ret

    align 16
large_string:
    lea r9, [rcx + 31]
    or rcx, 32-1
    sub r9, rcx

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [(rcx+1) + 32*0]
    vpmovmskb r8d, ymm1

    add rdx, r9
    cmp rdx, 32*4
    jbe first_vec_x4

    lea r10, [rcx + 1]
    or r10, 32*4-1
    sub r10, rcx

    shrx r8d, r8d, r9d

    xor r9, r9
    sub r9, r10
    lea rdx, [rdx + r9 + 32]

    popcnt r8d, r8d
    add rax, r8

    cmp r10, 32*1
    je aligned_vec_x4

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [(rcx+1) + 32*1]
    vpmovmskb r8d, ymm2
    popcnt r8d, r8d
    add rax, r8

    cmp r10, 32*2
    je aligned_vec_x4

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [(rcx+1) + 32*2]
    vpmovmskb r8d, ymm3
    popcnt r8d, r8d
    add rax, r8

    cmp r10, 32*3
    je aligned_vec_x4

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [(rcx+1) + 32*3]
    vpmovmskb r8d, ymm4
    popcnt r8d, r8d
    add rax, r8

aligned_vec_x4:

    add rcx, r10 ; align to 128 byte

    cmp rdx, 32*5
    jb last_vec_x4

    sub rdx, 32*5

    align 32
vec_x4_loop:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [(rcx+1) + 32*0]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [(rcx+1) + 32*1]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [(rcx+1) + 32*2]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [(rcx+1) + 32*3]
    vpmovmskb r8d, ymm1
    vpmovmskb r9d, ymm2
    vpmovmskb r10d, ymm3
    vpmovmskb r11d, ymm4
    popcnt r8d, r8d
    popcnt r9d, r9d
    popcnt r10d, r10d
    popcnt r11d, r11d
    add rax, r8
    add rax, r9
    add rax, r10
    add rax, r11

    sub rcx, -128
    sub rdx, 32*4
    jg vec_x4_loop

    add rdx, 32*5

    align 16
last_vec_x4:
    sub rdx, 32
    jle return_vzeroupper2

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [(rcx+1) + 32*0]
    vpmovmskb r8d, ymm1
    bzhi r8, r8, rdx
    popcnt r8d, r8d
    add rax, r8

    sub rdx, 32
    jle return_vzeroupper2

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [(rcx+1) + 32*1]
    vpmovmskb r8d, ymm2
    bzhi r8, r8, rdx
    popcnt r8d, r8d
    add rax, r8

    sub rdx, 32
    jle return_vzeroupper2

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [(rcx+1) + 32*2]
    vpmovmskb r8d, ymm3
    bzhi r8, r8, rdx
    popcnt r8d, r8d
    add rax, r8

    sub rdx, 32
    jle return_vzeroupper2

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [(rcx+1) + 32*3]
    vpmovmskb r8d, ymm4
    bzhi r8, r8, rdx
    popcnt r8d, r8d
    add rax, r8

return_vzeroupper2:
    vzeroupper
    ret

    align 16
cross_page_boundary:
    mov r8, rcx
    and rcx, -32
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rcx]
    vpmovmskb eax, ymm5

    lea r9, [rcx + 32]
    sub r9, r8

    shrx r10d, eax, r8d
    xor eax, eax
    popcnt eax, r10d
    bzhi r11d, r10d, edx

    sub rdx, r9
    jg large_string

    popcnt eax, r11d
    vzeroupper
    ret

betterstring_strcount_char_avx2 ENDP

_TEXT$align64 ENDS

END
