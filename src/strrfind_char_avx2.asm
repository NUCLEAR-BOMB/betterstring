
OPTION AVXENCODING:PREFER_VEX

extrn __ImageBase:byte

.code

DEBUGBREAK equ <int 3>

PAGE_SIZE equ 1 SHL 12 ; 4096

; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of string to compare
; char        character (r8b) - character to compare with
;
; this function uses AVX2 processor extension instructions 
betterstring_strrfind_char_avx2 PROC

    ; DEBUGBREAK

    xor rax, rax            ; set up return value in case it is not found

    cmp rdx, 32
    jb compare_small        ; count < 32

    ; DEBUGBREAK

    movzx r9d, r8b
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8

    je compare_vec_last     ; count == 32

    lea r9, [rcx + rdx]

    cmp rdx, 32*8
    jbe compare_vec_x8      ; count <= 32*8

    lea r8, [rcx + 32*8]
vec_x8_loop:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*1]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [r9 - 32*2]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*3]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [r9 - 32*4]
    vpor ymm1, ymm1, ymm2
    vpor ymm1, ymm1, ymm3
    vpor ymm4, ymm1, ymm4
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [r9 - 32*5]
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*6]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [r9 - 32*7]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*8]
    vpor ymm1, ymm1, ymm5
    vpor ymm1, ymm1, ymm2
    vpor ymm1, ymm1, ymm3
    vpor ymm1, ymm1, ymm4

    vptest ymm1, ymm1
    jnz vec_x8_match

    sub r9, 32*8
    cmp r9, r8
    jae vec_x8_loop

    mov rdx, r9
    sub rdx, rcx
    jz vzeroupper_return

    cmp rdx, 32
    jbe compare_vec_last

compare_vec_x8:

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*1]
    vpmovmskb r8, ymm1
    bsr r8d, r8d
    jnz vec_return1
    cmp rdx, 32*2
    jbe compare_vec_last

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [r9 - 32*2]
    vpmovmskb r8, ymm2
    bsr r8d, r8d
    jnz vec_return2
    cmp rdx, 32*3
    jbe compare_vec_last

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*3]
    vpmovmskb r8, ymm3
    bsr r8d, r8d
    jnz vec_return3
    cmp rdx, 32*4
    jbe compare_vec_last

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [r9 - 32*4]
    vpmovmskb r8, ymm4
    bsr r8d, r8d
    jnz vec_return4
    cmp rdx, 32*5
    jbe compare_vec_last

    vpcmpeqb ymm5, ymm0, YMMWORD PTR [r9 - 32*5]
    vpmovmskb r8, ymm5
    bsr r8d, r8d
    jnz vec_return5
    cmp rdx, 32*6
    jbe compare_vec_last

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*6]
    vpmovmskb r8, ymm1
    bsr r8d, r8d
    jnz vec_return6
    cmp rdx, 32*7
    jbe compare_vec_last

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*7]
    vpmovmskb r8, ymm3
    bsr r8d, r8d
    jnz vec_return7

compare_vec_last:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb r8, ymm1
    bsr r8, r8
    lea r8, [r8 + rcx]
    ; rax must be 0
    cmovnz rax, r8
    vzeroupper
    ret

vzeroupper_return:
    vzeroupper
    ; rax must be 0
    ret

vec_x8_match:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*1]
    vpmovmskb r8, ymm1
    bsr r8d, r8d
    jnz vec_return1

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [r9 - 32*2]
    vpmovmskb r8, ymm2
    bsr r8d, r8d
    jnz vec_return2

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*3]
    vpmovmskb r8, ymm3
    bsr r8d, r8d
    jnz vec_return3

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [r9 - 32*4]
    vpmovmskb r8, ymm4
    bsr r8d, r8d
    jnz vec_return4

    vpcmpeqb ymm5, ymm0, YMMWORD PTR [r9 - 32*5]
    vpmovmskb r8, ymm5
    bsr r8d, r8d
    jnz vec_return5

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*6]
    vpmovmskb r8, ymm1
    bsr r8d, r8d
    jnz vec_return6

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [r9 - 32*7]
    vpmovmskb r8, ymm2
    bsr r8d, r8d
    jnz vec_return7

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*8]
    vpmovmskb r8, ymm3
    bsr r8d, r8d
    lea r9, [r9 + r8 - 32*8]
    ; rax must be 0
    cmovnz rax, r9
    vzeroupper
    ret

vec_return1:
    lea rax, [r9 + r8 - 32*1]
    vzeroupper
    ret
vec_return2:
    lea rax, [r9 + r8 - 32*2]
    vzeroupper
    ret
vec_return3:
    lea rax, [r9 + r8 - 32*3]
    vzeroupper
    ret
vec_return4:
    lea rax, [r9 + r8 - 32*4]
    vzeroupper
    ret
vec_return5:
    lea rax, [r9 + r8 - 32*5]
    vzeroupper
    ret
vec_return6:
    lea rax, [r9 + r8 - 32*6]
    vzeroupper
    ret
vec_return7:
    lea rax, [r9 + r8 - 32*7]
    vzeroupper
    ret

compare_small:
    cmp rdx, 1
    jbe one_or_less

    mov r9, rcx
    and r9, PAGE_SIZE - 1
    cmp r9, PAGE_SIZE - 32  ; check if next 32 byte does cross page boundary
    jg page_cross

    movzx r9d, r8b
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb r9, ymm1
    xor r8, r8              ; clear all 64 bits from garbage
    bzhi r8d, r9d, edx
    bsr r8d, r8d            ; get offset to matching character
    lea r8, [r8 + rcx]      ; add offset to string pointer to get pointer to matching character
    ; rax must be 0
    cmovnz rax, r8          ; move pointer to matching character if it was found otherwise leave 0 (nullptr)
    vzeroupper
    ret

page_cross:
    cmp r8b, BYTE PTR [rcx + rdx - 1]
    je page_cross_exit
    dec rdx
    jnz page_cross
    ; rax must be 0
    ; no ymm register was modified
    ret

page_cross_exit:
    lea rax, [rcx + rdx - 1]
    ; no ymm register was modified
    ret

one_or_less:
    jb return_nullptr
    cmp r8b, BYTE PTR [rcx]
    ; rax must be 0
    cmove rax, rcx
return_nullptr:
    ; no ymm register was modified
    ret

betterstring_strrfind_char_avx2 ENDP
    
END
