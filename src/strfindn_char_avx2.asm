
OPTION AVXENCODING:PREFER_VEX

PAGE_SIZE equ 4096

.code

; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - length of string to compare
; char        character (r8b) - character to compare with
; Returns a pointer to the position of the first character that is not equal to the passed character
;
; NB: this function uses AVX2 and BMI2 processor extensions
betterstring_strfindn_char_avx2 PROC

    test rdx, rdx
    jz return_zero

    vmovd xmm0, r8d
    vpbroadcastb ymm0, xmm0 ; fill ymm0 with r8b (character)

    mov rax, rcx
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    ja cross_page

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb eax, ymm1     ; the upper bits of rax are filled with zeros
    not eax
    tzcnt eax, eax
    cmp eax, edx
    jae return_zero
    cmp eax, 32
    je align_more_vec

    vzeroupper
    add rax, rcx
    ret
return_zero:
    vzeroupper
    xor rax, rax
    ret

    align 16
align_more_vec:
cross_page_continue:
    ; int 3

    cmp rdx, 32*2
    jbe last_vec

    mov r8, rcx
    and rcx, -32            ; align down to 32-byte alignment rcx (string pointer)
    and r8, 32-1            ; calculate misaligned byte count
    add rdx, r8             ; correct rdx (string length)

    ; NB: should never read outside the string
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb eax, ymm1
    inc eax
    jnz return_vec1

    cmp rdx, 32*3
    jbe last_vec
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb eax, ymm2
    inc eax
    jnz return_vec2

    cmp rdx, 32*4
    jbe last_vec
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*3]
    vpmovmskb eax, ymm3
    inc eax
    jnz return_vec3

    cmp rdx, 32*5
    jbe last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*4]
    vpmovmskb eax, ymm4
    inc eax
    jnz return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe last_4x_vec

    ; int 3

    mov r8, rcx
    and rcx, -32*4          ; align down to 128-byte alignment rcx (string pointer)
    and r8, 32*4-1          ; calculate misaligned byte count
    add rdx, r8             ; correct rdx (string length)

    align 16
loop_4x_vec:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpand ymm5, ymm1, ymm2
    vpand ymm1, ymm3, ymm4
    vpand ymm1, ymm5, ymm1
    vpmovmskb eax, ymm1
    inc eax
    jnz loop_4x_vec_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja loop_4x_vec

    align 16
last_4x_vec:
    cmp rdx, 32*1
    jb last_vec
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpmovmskb eax, ymm1
    inc eax
    jnz loop_return_vec1

    cmp rdx, 32*2
    jb last_vec
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb eax, ymm2
    inc eax
    jnz loop_return_vec2

    cmp rdx, 32*3
    jb last_vec
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb eax, ymm3
    inc eax
    jnz loop_return_vec3

    jmp last_vec

    align 16
loop_4x_vec_return:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpmovmskb eax, ymm1
    inc eax
    jnz loop_return_vec1

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb eax, ymm2
    inc eax
    jnz loop_return_vec2

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb eax, ymm3
    inc eax
    jnz loop_return_vec3

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpmovmskb eax, ymm4
    inc eax

    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]
    ret

    align 16
return_vec1:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]
    ret

    align 16
return_vec2:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]
    ret

    align 16
return_vec3:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]
    ret

    align 16
return_vec4:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*4]
    ret

    align 16
loop_return_vec1:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*0]
    ret

    align 16
loop_return_vec2:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]
    ret

    align 16
loop_return_vec3:
    vzeroupper
    dec eax
    not eax
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]
    ret

    align 16
last_vec:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + rdx - 32]
    vpmovmskb r8d, ymm1
    not r8d
    vzeroupper
    test r8d, r8d
    jz last_vec_return_zero

    lea rax, [rcx + rdx - 32]
    tzcnt r8d, r8d
    add rax, r8
    ret
last_vec_return_zero:
    xor rax, rax
    ret

    align 16
cross_page:
    cmp rdx, 32
    jbe cross_page_less_vec
    
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb eax, ymm1
    not eax

    test eax, eax
    jz cross_page_continue
    vzeroupper

    tzcnt eax, eax
    add rax, rcx
    ret

    align 16
cross_page_less_vec:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + rdx - 32]
    vpmovmskb eax, ymm1
    not eax
    neg dl
    shrx eax, eax, edx

    test eax, eax
    jz return_zero

    vzeroupper
    tzcnt eax, eax
    add rax, rcx
    ret

betterstring_strfindn_char_avx2 ENDP

END
