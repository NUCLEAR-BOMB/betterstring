
; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence

    movzx r9d, BYTE PTR [r8] ; load first character (and the only one) in the character sequence
    vmovd xmm0, r9d
    vpbroadcastb ymm0, xmm0 ; _mm256_set1_epi8(needle[0])

    cmp rdx, 32
    ja cmp_1_large

    mov r10, rcx
    and r10, PAGE_SIZE-1
    cmp r10, PAGE_SIZE-32  ; check if next 32 byte does cross page boundary
    jg cmp_1_page_cross

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb eax, ymm1
    tzcnt r10d, eax
    xor rax, rax            ; set rax to 0 in case that string does not contain a character
    cmp r10d, edx           ; if string does not contain a character or it is outside the string
    lea r10, [r10 + rcx]  ; compute a pointer to a found character
    cmovb rax, r10

    vzeroupper
    ret

    align 16
cmp_1_page_cross:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + rdx - 32]
    vpmovmskb r9d, ymm1
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
cmp_1_large:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpmovmskb eax, ymm1
    test eax, eax
    jnz cmp_1_return_vec1

    cmp rdx, 32*2
    jbe cmp_1_last_vec
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb eax, ymm2
    test eax, eax
    jnz cmp_1_return_vec2

    cmp rdx, 32*3
    jbe cmp_1_last_vec
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb eax, ymm3
    test eax, eax
    jnz cmp_1_return_vec3

    cmp rdx, 32*4
    jbe cmp_1_last_vec
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpmovmskb eax, ymm4
    test eax, eax
    jnz cmp_1_return_vec4

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    jbe cmp_1_vec_loop_last

    align 16
cmp_1_vec_loop:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpor ymm5, ymm1, ymm2
    vpor ymm2, ymm3, ymm4
    vpor ymm1, ymm2, ymm5
    vpmovmskb eax, ymm1
    test eax, eax
    jnz cmp_1_vec_loop_return

    add rcx, 32*4
    sub rdx, 32*4
    cmp rdx, 32*4
    ja cmp_1_vec_loop

    align 16
cmp_1_vec_loop_last:

    cmp rdx, 32*1
    jbe cmp_1_last_vec
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpmovmskb eax, ymm1
    test eax, eax
    jnz cmp_1_return_vec1

    cmp rdx, 32*2
    jbe cmp_1_last_vec
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb eax, ymm2
    test eax, eax
    jnz cmp_1_return_vec2

    cmp rdx, 32*3
    jbe cmp_1_last_vec
    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb eax, ymm3
    test eax, eax
    jnz cmp_1_return_vec3

    align 16
cmp_1_last_vec:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + rdx - 32]
    vpmovmskb eax, ymm1
    tzcnt r10d, eax
    xor rax, rax
    cmp r10d, 32
    lea r10, [r10 + rcx - 32]
    lea r10, [r10 + rdx]
    cmovne rax, r10

    vzeroupper
    ret

    align 16
cmp_1_vec_loop_return:
    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx + 32*0]
    vpmovmskb eax, ymm1
    test eax, eax
    jnz cmp_1_return_vec1

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*1]
    vpmovmskb eax, ymm2
    test eax, eax
    jnz cmp_1_return_vec2

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rcx + 32*2]
    vpmovmskb eax, ymm3
    test eax, eax
    jnz cmp_1_return_vec3

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rcx + 32*3]
    vpmovmskb eax, ymm4
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]

    vzeroupper
    ret

    align 16
cmp_1_return_vec1:
    tzcnt eax, eax
    add rax, rcx
    vzeroupper
    ret

    align 16
cmp_1_return_vec2:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*1]
    vzeroupper
    ret

    align 16
cmp_1_return_vec3:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*2]
    vzeroupper
    ret

    align 16
cmp_1_return_vec4:
    tzcnt eax, eax
    lea rax, [rax + rcx + 32*3]
    vzeroupper
    ret
