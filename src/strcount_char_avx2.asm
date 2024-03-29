
; // Copyright 2024.
; // Distributed under the Boost Software License, Version 1.0.
; // (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

OPTION AVXENCODING:PREFER_VEX

PAGE_SIZE equ 1 SHL 12 ; 4096

.code

; const char* string (rcx) - pointer to string in which to count
; size_t      count (rdx) - lenght of string
; char        character (r8b) - character to count
; returns: size_t (rax) - number of characters counted
;
; Note: this function uses AVX2, BMI2, POPCNT processor extensions
;
; bs::strcount algorithm:
;   1. If count <= 32 and next 32 byte does NOT cross page boundary,
;      count number of matches using YMM register.
;   2. If count <= 32 and next 32 byte DOES cross page boundary,
;      load chunk into YMM that starts before string, but ends right at the end of the string,
;      discard out of bounds matches, and then count the final number of character matches
;   3. If count <= 128 (32*4), continuously count with 32 byte chunks,
;      and in the end discard overlapping values.
;   4. If count > 128 (32*4), use unrolled loop, that processes 128 bytes at single cycle.

betterstring_strcount_char_avx2 PROC

    ; int 3

    xor rax, rax

    cmp rdx, 32
    jbe small

    ; int 3

    movzx r9d, r8b
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8(character)

    cmp rdx, 32*4
    jbe vec_x4

    lea rdx, [rcx + rdx - 32*4]

vec_loop:

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpmovmskb r8, ymm1
    vpmovmskb r9, ymm2
    vpmovmskb r10, ymm3
    vpmovmskb r11, ymm4
    popcnt r8, r8
    popcnt r9, r9
    popcnt r10, r10
    popcnt r11, r11
    add rax, r8
    add rax, r9
    add rax, r10
    add rax, r11

    add rcx, 32*4
    cmp rcx, rdx
    jb vec_loop

    add rdx, 32*4
    sub rdx, rcx
    jz vzeroupper_return

    cmp rdx, 32
    jbe vec_last

vec_x4:

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpmovmskb r8, ymm1
    xor r9d, r9d
    popcnt r9, r8
    add rax, r9
    cmp rdx, 32*2
    jbe vec_last

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb r8, ymm1
    xor r9d, r9d
    popcnt r9, r8
    add rax, r9
    cmp rdx, 32*3
    jbe vec_last

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb r8, ymm1
    xor r9d, r9d
    popcnt r9, r8
    add rax, r9

vec_last:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + rdx - 32] ; load last 32 bytes
    vpmovmskb r8, ymm1

    ; clear overlapping bits
    neg dl                  ; rdx = 32 - rdx
    shrx r8d, r8d, edx

    xor r9d, r9d
    popcnt r9, r8
    add rax, r9

vzeroupper_return:
    vzeroupper
    ret

    align 16
small:
    cmp rdx, 1
    jbe one_or_less

    movzx r9d, r8b
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8(character)

    mov r9, rcx
    and r9, PAGE_SIZE - 1
    cmp r9, PAGE_SIZE - 32  ; check if next 32 byte does cross page boundary
    ja vec_last

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb r9, ymm1

    bzhi r8, r9, rdx        ; discard comparisons of characters beyond the end of the string

    xor eax, eax
    popcnt rax, r8

    vzeroupper
    ret

    align 16
one_or_less:
    jb return_nullptr
    cmp r8b, BYTE PTR [rcx]
    sete al                 ; set 1 to rax if first character is matches
return_nullptr:
    ; no ymm register was modified
    ret

betterstring_strcount_char_avx2 ENDP

END
