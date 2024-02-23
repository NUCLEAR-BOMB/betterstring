
OPTION AVXENCODING:PREFER_VEX

.code

DEBUGBREAK equ <int 3>

PAGE_SIZE equ 1 SHL 12 ; 4096

; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - length of string to compare
; char        character (r8b) - character to compare with
;
; this function uses AVX2 processor extension instructions 
betterstring_strrfind_char_avx2 PROC

    ; DEBUGBREAK

    xor rax, rax            ; set up return value in case it is not found

    cmp rdx, 32
    jb compare_small        ; if count < 32 go to the handling of small string

    ; DEBUGBREAK

    movzx r9d, r8b          ;
    vmovd xmm0, r9d         ;
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8(character)

    je compare_vec_last     ; count == 32

    lea r9, [rcx + rdx]     ; r9 is past-the-end pointer

    cmp rdx, 32*8
    jbe compare_vec_x8      ; count <= 32*8

    lea r8, [rcx + 32*8]    ; if r9 >= r8 when its safe to read next 256 bytes
                            ; otherwise end the loop and go to handling the string < 256 bytes

    sub rsp, 32
    vmovdqu YMMWORD PTR [rsp], ymm6 ; save ymm6 due to that it is nonvolatile in Windows x64 ABI

vec_x8_loop:
    ; vpor      Latency - 1 Throughput - 0.33
    ; vpcmpeqb  Latency - 1 Throughput - 0.5

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r9 - 32*1]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [r9 - 32*2]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [r9 - 32*3]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [r9 - 32*4]
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [r9 - 32*5]
    vpcmpeqb ymm6, ymm0, YMMWORD PTR [r9 - 32*6]
    vpor ymm1, ymm1, ymm2
    vpor ymm2, ymm3, ymm4
    vpor ymm3, ymm5, ymm6
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [r9 - 32*7]
    vpcmpeqb ymm6, ymm0, YMMWORD PTR [r9 - 32*8]
    vpor ymm4, ymm5, ymm6
    vpor ymm5, ymm1, ymm2
    vpor ymm6, ymm3, ymm4
    vpor ymm1, ymm5, ymm6
    vptest ymm1, ymm1
    jnz vec_x8_match        ; if ymm1 is not filled with zeros

    sub r9, 32*8
    cmp r9, r8              ; check if next loop cycle will cause out-of-bounds reading
    jae vec_x8_loop

    vmovdqu ymm6, YMMWORD PTR [rsp] ; restore ymm6
    add rsp, 32

    mov rdx, r9
    sub rdx, rcx            ; calculate remaining length
    jz vzeroupper_return

    cmp rdx, 32
    jbe compare_vec_last    ; if the remaining length is <= 32, compare beginning of the string

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

    ; compare remaining bytes 
compare_vec_last:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb r8, ymm1
    bsr r8, r8
    lea r8, [r8 + rcx]
    ; rax must be 0
    cmovnz rax, r8
    vzeroupper
    ret

    align 16
vzeroupper_return:
    vzeroupper
    ; rax must be 0
    ret

    align 16
vec_x8_match:
    vmovdqu ymm6, YMMWORD PTR [rsp] ; restore ymm6
    add rsp, 32

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
    lea rax, [r9 + r8 - 32*8]
    ; we know that this chunk contains matching character, and this is the last comparison,
    ; so the cmov instruction is not needed
    vzeroupper
    ret

    align 16
vec_return1:
    lea rax, [r9 + r8 - 32*1]
    vzeroupper
    ret
    align 16
vec_return2:
    lea rax, [r9 + r8 - 32*2]
    vzeroupper
    ret
    align 16
vec_return3:
    lea rax, [r9 + r8 - 32*3]
    vzeroupper
    ret
    align 16
vec_return4:
    lea rax, [r9 + r8 - 32*4]
    vzeroupper
    ret
    align 16
vec_return5:
    lea rax, [r9 + r8 - 32*5]
    vzeroupper
    ret
    align 16
vec_return6:
    lea rax, [r9 + r8 - 32*6]
    vzeroupper
    ret
    align 16
vec_return7:
    lea rax, [r9 + r8 - 32*7]
    vzeroupper
    ret

    align 16
compare_small:
    cmp rdx, 1
    jbe one_or_less

    movzx r9d, r8b
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0

    mov r9, rcx
    and r9, PAGE_SIZE - 1
    cmp r9, PAGE_SIZE - 32  ; check if next 32 byte does cross page boundary
    jg page_cross

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb r9, ymm1
    bzhi r8, r9, rdx
    bsr r8, r8              ; get offset to matching character
    lea r8, [r8 + rcx]      ; add offset to string pointer to get pointer to matching character
    ; rax must be 0
    cmovnz rax, r8          ; move pointer to matching character if it was found otherwise leave 0 (nullptr)
    vzeroupper
    ret

    align 16
page_cross:
    lea r8, [rcx + rdx - 32]
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [r8]
    vpmovmskb r9, ymm1

    bsr r9d, r9d
    lea r9, [r9 + r8]
    ; rax must be 0
    cmovz r9, rax           ; if the character was not found in 32 bytes chunk, set r9 to 0
    cmp r9, rcx
    cmovae rax, r9          ; if the match is in the range AND the character exists in the chunk, set rax to found location

    vzeroupper
    ret

    align 16
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
