
; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence

CMP_6_COMPARE_YMM MACRO msk_reg:REQ, load_loc:REQ
    vpcmpeqb ymm6, ymm0, load_loc
    vpcmpeqb ymm7, ymm1, load_loc
    vpcmpeqb ymm8, ymm2, load_loc
    vpcmpeqb ymm9, ymm3, load_loc
    vpcmpeqb ymm10, ymm4, load_loc
    vpcmpeqb ymm11, ymm5, load_loc
    vpor ymm6, ymm6, ymm7
    vpor ymm8, ymm8, ymm9
    vpor ymm10, ymm10, ymm11
    vpor ymm6, ymm6, ymm8
    vpor ymm10, ymm6, ymm10
    vpmovmskb msk_reg, ymm10
ENDM

CMP_6_COMPARE_YMM_LAST MACRO msk_reg:REQ, load_loc:REQ
    vpcmpeqb ymm0, ymm0, load_loc
    vpcmpeqb ymm1, ymm1, load_loc
    vpcmpeqb ymm2, ymm2, load_loc
    vpcmpeqb ymm3, ymm3, load_loc
    vpcmpeqb ymm4, ymm4, load_loc
    vpcmpeqb ymm5, ymm5, load_loc
    vpor ymm0, ymm0, ymm1
    vpor ymm2, ymm2, ymm3
    vpor ymm4, ymm4, ymm5
    vpor ymm0, ymm0, ymm2
    vpor ymm4, ymm0, ymm4
    vpmovmskb msk_reg, ymm4
ENDM

    MM256_SET1_EPI8 ymm0, BYTE PTR [r8 + 0]
    MM256_SET1_EPI8 ymm1, BYTE PTR [r8 + 1]
    MM256_SET1_EPI8 ymm2, BYTE PTR [r8 + 2]
    MM256_SET1_EPI8 ymm3, BYTE PTR [r8 + 3]
    MM256_SET1_EPI8 ymm4, BYTE PTR [r8 + 4]
    MM256_SET1_EPI8 ymm5, BYTE PTR [r8 + 5]

    cmp rdx, 32
    ja cmp_6_large

    mov r10, rcx
    and r10, PAGE_SIZE-1
    cmp r10, PAGE_SIZE-32  ; check if next 32 byte does cross page boundary
    jg cmp_6_page_cross

    CMP_6_COMPARE_YMM_LAST eax, YMMWORD PTR [rcx]
    tzcnt r10d, eax
    xor rax, rax            ; set rax to 0 in case that string does not contain a character
    cmp r10d, edx           ; if string does not contain a character or it is outside the string
    lea r10, [r10 + rcx]    ; compute a pointer to a found character
    cmovb rax, r10

    vzeroupper
    ret

    align 16
cmp_6_page_cross:
    CMP_6_COMPARE_YMM_LAST r9d, YMMWORD PTR [rcx + rdx - 32]
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
cmp_6_large:
    PUSH_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11

    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_6_return_vec1

    cmp rdx, 32*2
    jbe cmp_6_last_vec
    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_6_return_vec2

    cmp rdx, 32*3
    jbe cmp_6_last_vec
    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_6_return_vec3

    cmp rdx, 32*4
    jbe cmp_6_last_vec
    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    test eax, eax
    jnz cmp_6_return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe cmp_6_vec_loop_last

    PUSH_XMM xmm12

    align 16
cmp_6_vec_loop:
    vpcmpeqb ymm6, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm7, ymm1, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm8, ymm2, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm9, ymm3, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm10, ymm4, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm11, ymm5, YMMWORD PTR [rcx + 32*0]
    vpor ymm6, ymm6, ymm7
    vpor ymm8, ymm8, ymm9
    vpor ymm10, ymm10, ymm11
    vpor ymm6, ymm6, ymm8
    vpor ymm12, ymm6, ymm10
    vpcmpeqb ymm6, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm7, ymm1, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm8, ymm2, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm9, ymm3, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm10, ymm4, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm11, ymm5, YMMWORD PTR [rcx + 32*1]
    vpor ymm6, ymm6, ymm7
    vpor ymm8, ymm8, ymm9
    vpor ymm10, ymm10, ymm11
    vpor ymm6, ymm6, ymm8
    vpor ymm10, ymm10, ymm12
    vpor ymm12, ymm6, ymm10
    vpcmpeqb ymm6, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm7, ymm1, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm8, ymm2, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm9, ymm3, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm10, ymm4, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm11, ymm5, YMMWORD PTR [rcx + 32*2]
    vpor ymm6, ymm6, ymm7
    vpor ymm8, ymm8, ymm9
    vpor ymm10, ymm10, ymm11
    vpor ymm6, ymm6, ymm8
    vpor ymm10, ymm10, ymm12
    vpor ymm12, ymm6, ymm10
    vpcmpeqb ymm6, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm7, ymm1, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm8, ymm2, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm9, ymm3, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm10, ymm4, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm11, ymm5, YMMWORD PTR [rcx + 32*3]
    vpor ymm6, ymm6, ymm7
    vpor ymm8, ymm8, ymm9
    vpor ymm10, ymm10, ymm11
    vpor ymm6, ymm6, ymm8
    vpor ymm10, ymm10, ymm12
    vpor ymm12, ymm6, ymm10

    vpmovmskb eax, ymm12
    test eax, eax
    jnz cmp_6_vec_loop_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja cmp_6_vec_loop

    POP_XMM xmm12

    align 16
cmp_6_vec_loop_last:

    cmp rdx, 32*1
    jbe cmp_6_last_vec
    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_6_return_vec1

    cmp rdx, 32*2
    jbe cmp_6_last_vec
    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_6_return_vec2

    cmp rdx, 32*3
    jbe cmp_6_last_vec
    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_6_return_vec3

    align 16
cmp_6_last_vec:
    CMP_6_COMPARE_YMM_LAST eax, YMMWORD PTR [rcx + rdx - 32]
    tzcnt r10d, eax
    xor rax, rax
    cmp r10d, 32
    lea r10, [r10 + rcx - 32]
    lea r10, [r10 + rdx]
    cmovne rax, r10

    POP_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11
    vzeroupper
    ret

    align 16
cmp_6_vec_loop_return:
    POP_XMM xmm12

    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_6_return_vec1

    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_6_return_vec2

    CMP_6_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_6_return_vec3

    POP_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11
    CMP_6_COMPARE_YMM_LAST eax, YMMWORD PTR [rcx + 32*3]
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    vzeroupper
    ret

    align 16
cmp_6_return_vec1:
    tzcnt eax, eax
    add rax, rcx

    POP_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11
    vzeroupper
    ret

    align 16
cmp_6_return_vec2:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]

    POP_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11
    vzeroupper
    ret

    align 16
cmp_6_return_vec3:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]

    POP_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11
    vzeroupper
    ret

    align 16
cmp_6_return_vec4:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    POP_XMM xmm6, xmm7, xmm8, xmm9, xmm10, xmm11
    vzeroupper
    ret
