
; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence

CMP_ANY_COMPARE_YMM MACRO msk_reg:REQ, load_loc:REQ
    LOCAL compare_loop

    movzx r10d, BYTE PTR [r8]
    vmovd xmm0, r10d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8

    vpcmpeqb ymm1, ymm0, load_loc

    lea r11, [r9 - 1]
    align 16
compare_loop:
    movzx r10d, BYTE PTR [r8 + r11]
    vmovd xmm0, r10d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8

    vpcmpeqb ymm2, ymm0, load_loc
    vpor ymm1, ymm1, ymm2

    dec r11
    jnz compare_loop

    vpmovmskb msk_reg, ymm1
ENDM

    cmp rdx, 32
    ja cmp_any_large

    mov r10, rcx
    and r10, PAGE_SIZE-1
    cmp r10, PAGE_SIZE-32  ; check if next 32 byte does cross page boundary
    jg cmp_any_page_cross

    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx]
    tzcnt r10d, eax
    xor rax, rax            ; set rax to 0 in case that string does not contain a character
    cmp r10d, edx           ; if string does not contain a character or it is outside the string
    lea r10, [r10 + rcx]  ; compute a pointer to a found character
    cmovb rax, r10

    vzeroupper
    ret

    align 16
cmp_any_page_cross:
    CMP_ANY_COMPARE_YMM r9d, YMMWORD PTR [rcx + rdx - 32]
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
cmp_any_large:
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_any_return_vec1

    cmp rdx, 32*2
    jbe cmp_any_last_vec
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_any_return_vec2

    cmp rdx, 32*3
    jbe cmp_any_last_vec
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_any_return_vec3

    cmp rdx, 32*4
    jbe cmp_any_last_vec
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    test eax, eax
    jnz cmp_any_return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe cmp_any_vec_loop_last

    align 16
cmp_any_vec_loop:

    vpxor ymm1, ymm1, ymm1
    xor r11, r11
cmp_any_vec_loop_compare:
    movzx r10d, BYTE PTR [r8 + r11]
    vmovd xmm0, r10d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rcx + 32*3]
    vpor ymm2, ymm2, ymm3
    vpor ymm4, ymm4, ymm5
    vpor ymm2, ymm2, ymm1
    vpor ymm1, ymm2, ymm4

    inc r11
    cmp r11, r9
    jb cmp_any_vec_loop_compare

    vpmovmskb eax, ymm1
    test eax, eax
    jnz cmp_any_vec_loop_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja cmp_any_vec_loop

    align 16
cmp_any_vec_loop_last:

    cmp rdx, 32*1
    jbe cmp_any_last_vec
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_any_return_vec1

    cmp rdx, 32*2
    jbe cmp_any_last_vec
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_any_return_vec2

    cmp rdx, 32*3
    jbe cmp_any_last_vec
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_any_return_vec3

    align 16
cmp_any_last_vec:
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + rdx - 32]
    tzcnt r10d, eax
    xor rax, rax
    cmp r10d, 32
    lea r10, [r10 + rcx - 32]
    lea r10, [r10 + rdx]
    cmovne rax, r10

    vzeroupper
    ret

    align 16
cmp_any_vec_loop_return:
    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*0]
    test eax, eax
    jnz cmp_any_return_vec1

    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*1]
    test eax, eax
    jnz cmp_any_return_vec2

    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*2]
    test eax, eax
    jnz cmp_any_return_vec3

    CMP_ANY_COMPARE_YMM eax, YMMWORD PTR [rcx + 32*3]
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    vzeroupper
    ret

    align 16
cmp_any_return_vec1:
    tzcnt eax, eax
    add rax, rcx

    vzeroupper
    ret

    align 16
cmp_any_return_vec2:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]

    vzeroupper
    ret

    align 16
cmp_any_return_vec3:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]

    vzeroupper
    ret

    align 16
cmp_any_return_vec4:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    vzeroupper
    ret
