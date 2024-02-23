
; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence

    align 16
cmp_4:
    movzx r9d, BYTE PTR [r8 + 0]
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8(needle[0])
    movzx r9d, BYTE PTR [r8 + 1]
    vmovd xmm1, r9d
    vpbroadcastb ymm1, xmm1 ; _mm256_set1_epi8(needle[1])
    movzx r9d, BYTE PTR [r8 + 2]
    vmovd xmm2, r9d
    vpbroadcastb ymm2, xmm2 ; _mm256_set1_epi8(needle[2])
    movzx r9d, BYTE PTR [r8 + 3]
    vmovd xmm3, r9d
    vpbroadcastb ymm3, xmm3 ; _mm256_set1_epi8(needle[3])

    cmp rdx, 32
    ja cmp_4_large

    mov r10, rcx
    and r10, PAGE_SIZE-1
    cmp r10, PAGE_SIZE-32  ; check if next 32 byte does cross page boundary
    jg cmp_4_page_cross

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    tzcnt r10d, eax
    xor rax, rax            ; set rax to 0 in case that string does not contain a character
    cmp r10d, edx           ; if string does not contain a character or it is outside the string
    lea r10, [r10 + rcx]  ; compute a pointer to a found character
    cmovb rax, r10

    vzeroupper
    ret

    align 16
cmp_4_page_cross:
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + rdx - 32]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + rdx - 32]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + rdx - 32]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + rdx - 32]
    vpor ymm4, ymm4, ymm5
    vpmovmskb r9d, ymm4
    neg dl
    shrx r9d, r9d, edx
    tzcnt r9d, r9d
    xor rax, rax
    cmp r9d, 32
    lea r9, [r9 + rcx]
    cmovne rax, r9

    vzeroupper
    ret

    align 16
cmp_4_large:
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec1

    cmp rdx, 32*2
    jbe cmp_4_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec2

    cmp rdx, 32*3
    jbe cmp_4_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec3

    cmp rdx, 32*4
    jbe cmp_4_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe cmp_4_vec_loop_last

    ; ymm6, ymm7, ymm8
    sub rsp, 16*3
    vmovdqu XMMWORD PTR [rsp + 16*0], xmm6
    vmovdqu XMMWORD PTR [rsp + 16*1], xmm7
    vmovdqu XMMWORD PTR [rsp + 16*2], xmm8

    align 16
cmp_4_vec_loop:
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm6, ymm2, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm7, ymm3, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpor ymm6, ymm6, ymm7
    vpor ymm8, ymm4, ymm6
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm6, ymm2, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm7, ymm3, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpor ymm6, ymm6, ymm7
    vpor ymm4, ymm4, ymm8
    vpor ymm8, ymm4, ymm6
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm6, ymm2, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm7, ymm3, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpor ymm6, ymm6, ymm7
    vpor ymm4, ymm4, ymm8
    vpor ymm8, ymm4, ymm6
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm6, ymm2, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm7, ymm3, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpor ymm6, ymm6, ymm7
    vpor ymm4, ymm4, ymm8
    vpor ymm8, ymm4, ymm6

    vpmovmskb eax, ymm8
    test eax, eax
    jnz cmp_4_vec_loop_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja cmp_4_vec_loop

    vmovdqu xmm6, XMMWORD PTR [rsp + 16*0]
    vmovdqu xmm7, XMMWORD PTR [rsp + 16*1]
    vmovdqu xmm8, XMMWORD PTR [rsp + 16*2]
    add rsp, 16*3

    align 16
cmp_4_vec_loop_last:

    cmp rdx, 32*1
    jbe cmp_4_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec1

    cmp rdx, 32*2
    jbe cmp_4_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec2

    cmp rdx, 32*3
    jbe cmp_4_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec3

    align 16
cmp_4_last_vec:
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + rdx - 32]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + rdx - 32]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + rdx - 32]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + rdx - 32]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    tzcnt r10d, eax
    xor rax, rax
    cmp r10d, 32
    lea r10, [r10 + rcx - 32]
    lea r10, [r10 + rdx]
    cmovne rax, r10

    vzeroupper
    ret

    align 16
cmp_4_vec_loop_return:
    vmovdqu xmm6, XMMWORD PTR [rsp + 16*0]
    vmovdqu xmm7, XMMWORD PTR [rsp + 16*1]
    vmovdqu xmm8, XMMWORD PTR [rsp + 16*2]
    add rsp, 16*3

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*0]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec1

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*1]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec2

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*2]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_4_return_vec3

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm5, ymm1, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpcmpeqb ymm5, ymm3, YMMWORD PTR [rcx + 32*3]
    vpor ymm4, ymm4, ymm5
    vpmovmskb eax, ymm4
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    vzeroupper
    ret

    align 16
cmp_4_return_vec1:
    tzcnt eax, eax
    add rax, rcx
    vzeroupper
    ret

    align 16
cmp_4_return_vec2:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]
    vzeroupper
    ret

    align 16
cmp_4_return_vec3:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]
    vzeroupper
    ret

    align 16
cmp_4_return_vec4:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]
    vzeroupper
    ret
