
; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence

CMP_5_COMPARE_YMM MACRO msk_reg:REQ, load_loc:REQ
    vpcmpeqb ymm5, ymm0, load_loc
    vpcmpeqb ymm6, ymm1, load_loc
    vpcmpeqb ymm7, ymm2, load_loc
    vpcmpeqb ymm8, ymm3, load_loc
    vpcmpeqb ymm9, ymm4, load_loc
    vpor ymm5, ymm5, ymm6
    vpor ymm7, ymm7, ymm8
    vpor ymm9, ymm5, ymm9
    vpor ymm5, ymm9, ymm7
    vpmovmskb msk_reg, ymm5
ENDM

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
    movzx r9d, BYTE PTR [r8 + 4]
    vmovd xmm4, r9d
    vpbroadcastb ymm4, xmm4 ; _mm256_set1_epi8(needle[4])

    PUSH_XMM xmm6, xmm7, xmm8, xmm9

    cmp rdx, 32
    ja cmp_5_large

    mov r10, rcx
    and r10, PAGE_SIZE-1
    cmp r10, PAGE_SIZE-32  ; check if next 32 byte does cross page boundary
    jg cmp_5_page_cross

    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx]
    tzcnt r10d, eax
    xor rax, rax            ; set rax to 0 in case that string does not contain a character
    cmp r10d, edx           ; if string does not contain a character or it is outside the string
    lea r10, [r10 + rcx]  ; compute a pointer to a found character
    cmovb rax, r10

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_page_cross:
    CMP_5_COMPARE_YMM r9d, YMMWORD PTR [rcx + rdx - 32]
    neg dl
    shrx r9d, r9d, edx
    tzcnt r9d, r9d
    xor rax, rax
    cmp r9d, 32
    lea r9, [r9 + rcx]
    cmovne rax, r9

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_large:
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_5_return_vec1

    cmp rdx, 32*2
    jbe cmp_5_last_vec
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_5_return_vec2

    cmp rdx, 32*3
    jbe cmp_5_last_vec
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_5_return_vec3

    cmp rdx, 32*4
    jbe cmp_5_last_vec
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    test eax, eax
    jnz cmp_5_return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe cmp_5_vec_loop_last

    PUSH_XMM xmm10

    align 16
cmp_5_vec_loop:
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm6, ymm1, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm7, ymm2, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm8, ymm3, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm9, ymm4, YMMWORD PTR [rcx + 32*0]
    vpor ymm5, ymm5, ymm6
    vpor ymm7, ymm7, ymm8
    vpor ymm9, ymm5, ymm9
    vpor ymm10, ymm9, ymm7
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm6, ymm1, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm7, ymm2, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm8, ymm3, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm9, ymm4, YMMWORD PTR [rcx + 32*1]
    vpor ymm5, ymm5, ymm6
    vpor ymm7, ymm7, ymm8
    vpor ymm9, ymm9, ymm10
    vpor ymm10, ymm5, ymm7
    vpor ymm10, ymm10, ymm9
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm6, ymm1, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm7, ymm2, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm8, ymm3, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm9, ymm4, YMMWORD PTR [rcx + 32*2]
    vpor ymm5, ymm5, ymm6
    vpor ymm7, ymm7, ymm8
    vpor ymm9, ymm9, ymm10
    vpor ymm10, ymm5, ymm7
    vpor ymm10, ymm10, ymm9
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm6, ymm1, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm7, ymm2, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm8, ymm3, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm9, ymm4, YMMWORD PTR [rcx + 32*3]
    vpor ymm5, ymm5, ymm6
    vpor ymm7, ymm7, ymm8
    vpor ymm9, ymm9, ymm10
    vpor ymm10, ymm5, ymm7
    vpor ymm10, ymm10, ymm9

    vpmovmskb eax, ymm10
    test eax, eax
    jnz cmp_5_vec_loop_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja cmp_5_vec_loop

    POP_XMM xmm10

    align 16
cmp_5_vec_loop_last:

    cmp rdx, 32*1
    jbe cmp_5_last_vec
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_5_return_vec1

    cmp rdx, 32*2
    jbe cmp_5_last_vec
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_5_return_vec2

    cmp rdx, 32*3
    jbe cmp_5_last_vec
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_5_return_vec3

    align 16
cmp_5_last_vec:
    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + rdx - 32]
    tzcnt r10d, eax
    xor rax, rax
    cmp r10d, 32
    lea r10, [r10 + rcx - 32]
    lea r10, [r10 + rdx]
    cmovne rax, r10

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_vec_loop_return:
    POP_XMM xmm10

    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_5_return_vec1

    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_5_return_vec2

    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_5_return_vec3

    CMP_5_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_return_vec1:
    tzcnt eax, eax
    add rax, rcx

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_return_vec2:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_return_vec3:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret

    align 16
cmp_5_return_vec4:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    POP_XMM xmm6, xmm7, xmm8, xmm9
    vzeroupper
    ret
