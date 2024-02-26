
; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence

CMP_3_COMPARE_YMM MACRO out_reg:REQ, load_loc:REQ
    vpcmpeqb ymm3, ymm0, load_loc
    vpcmpeqb ymm4, ymm1, load_loc
    vpcmpeqb ymm5, ymm2, load_loc
    vpor ymm3, ymm3, ymm4
    vpor ymm4, ymm3, ymm5
    vpmovmskb out_reg, ymm4
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

    cmp rdx, 32
    ja cmp_3_large

    mov r10, rcx
    and r10, PAGE_SIZE-1
    cmp r10, PAGE_SIZE-32  ; check if next 32 byte does cross page boundary
    jg cmp_3_page_cross

    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx]
    tzcnt r10d, eax
    xor rax, rax            ; set rax to 0 in case that string does not contain a character
    cmp r10d, edx           ; if string does not contain a character or it is outside the string
    lea r10, [r10 + rcx]  ; compute a pointer to a found character
    cmovb rax, r10

    vzeroupper
    ret

    align 16
cmp_3_page_cross:
    CMP_3_COMPARE_YMM r9d, YMMWORD PTR [rcx + rdx - 32]
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
cmp_3_large:
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_3_return_vec1

    cmp rdx, 32*2
    jbe cmp_3_last_vec
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_3_return_vec2

    cmp rdx, 32*3
    jbe cmp_3_last_vec
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_3_return_vec3

    cmp rdx, 32*4
    jbe cmp_3_last_vec
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    test eax, eax
    jnz cmp_3_return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe cmp_3_vec_loop_last

    PUSH_XMM xmm6

    align 16
cmp_3_vec_loop:
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*0]
    vpor ymm3, ymm3, ymm4
    vpor ymm6, ymm3, ymm5
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*1]
    vpor ymm3, ymm3, ymm4
    vpor ymm4, ymm6, ymm5
    vpor ymm6, ymm3, ymm4
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*2]
    vpor ymm3, ymm3, ymm4
    vpor ymm4, ymm6, ymm5
    vpor ymm6, ymm3, ymm4
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm4, ymm1, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm5, ymm2, YMMWORD PTR [rcx + 32*3]
    vpor ymm3, ymm3, ymm4
    vpor ymm4, ymm6, ymm5
    vpor ymm6, ymm3, ymm4

    vpmovmskb eax, ymm6
    test eax, eax
    jnz cmp_3_vec_loop_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja cmp_3_vec_loop

    POP_XMM xmm6

    align 16
cmp_3_vec_loop_last:

    cmp rdx, 32*1
    jbe cmp_3_last_vec
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_3_return_vec1

    cmp rdx, 32*2
    jbe cmp_3_last_vec
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_3_return_vec2

    cmp rdx, 32*3
    jbe cmp_3_last_vec
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_3_return_vec3

    align 16
cmp_3_last_vec:
    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + rdx - 32]
    tzcnt r10d, eax
    xor rax, rax
    cmp r10d, 32
    lea r10, [r10 + rcx - 32]
    lea r10, [r10 + rdx]
    cmovne rax, r10

    vzeroupper
    ret

    align 16
cmp_3_vec_loop_return:
    POP_XMM xmm6

    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_3_return_vec1

    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_3_return_vec2

    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_3_return_vec3

    CMP_3_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    vzeroupper
    ret

    align 16
cmp_3_return_vec1:
    tzcnt eax, eax
    add rax, rcx
    vzeroupper
    ret

    align 16
cmp_3_return_vec2:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]
    vzeroupper
    ret

    align 16
cmp_3_return_vec3:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]
    vzeroupper
    ret

    align 16
cmp_3_return_vec4:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]
    vzeroupper
    ret
