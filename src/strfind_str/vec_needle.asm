
L MACRO label_name:REQ
    EXITM <@CatStr(<vec_needle_>, label_name)>
ENDM

    ; r9 (needle length) <= 32
    ; r9 (needle length) <= rdx (haystack length)

    lea rax, [rcx + rdx]
    sub rax, r9
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    jae L(cross_page)

    vmovdqu ymm5, YMMWORD PTR [r8]

    ; rdx (haystack length) >= 32
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1]

    mov eax, -1
    bzhi r8d, eax, r9d ; r8d = (1 << r9d) - 1. r9d - needle length

    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4
    
L(vec1_loop):
    test eax, eax
    jz L(vec1_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(vec1_loop)

    lea rax, [rcx + r10] ; return match position
    vzeroupper
    ret

L(vec1_loop_end):
    lea rax, [r9 + 64 - 1]
    cmp rdx, rax
    jbe L(vec_last)

    ; check next 32 bytes
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1 + 32]
    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4
L(vec2_loop):
    test eax, eax
    jz L(vec2_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10 + 32]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(vec2_loop)

    lea rax, [rcx + r10 + 32] ; return match position
    vzeroupper
    ret

L(vec2_loop_end):
    cmp rdx, 32*3
    jbe L(vec_last)

    align 16
L(vec_last):
    ; int 3

    lea rcx, [rcx + rdx - 32]
    neg r9
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + r9 + 1]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx]
    lea rcx, [rcx + r9 + 1]

    and rdx, 32-1
    neg rdx

    mov eax, -1
    shlx r10d, eax, edx

    vpand ymm4, ymm2, ymm3

    vpmovmskb eax, ymm4
    and eax, r10d ; filter out already checked characters
L(vec_last_loop):
    test eax, eax
    jz L(vec_last_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(vec_last_loop)

    lea rax, [rcx + r10] ; return match position
    vzeroupper
    ret

L(vec_last_loop_end):
    xor rax, rax
    vzeroupper
    ret

    align 16
L(cross_page):
    vmovdqu ymm5, YMMWORD PTR [r8 + r9 - 32]

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1]
    add rcx, r9

    xor r8, r8
    mov eax, -1
    sub r8, r9
    shlx r8d, eax, r8d

    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4
    
L(cross_page_vec1_loop):
    test eax, eax
    jz L(cross_page_vec1_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10 - 32]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(cross_page_vec1_loop)

    lea rax, [rcx + r10] ; return match position
    sub rax, r9
    vzeroupper
    ret

L(cross_page_vec1_loop_end):
    sub rcx, r9
    lea rax, [r9 + 32*2 - 1]
    cmp rdx, rax
    jbe L(cross_page_vec_last)

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1 + 32]
    add rcx, r9
    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4

L(cross_page_vec2_loop):
    test eax, eax
    jz L(cross_page_vec2_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10 - 32 + 32]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(cross_page_vec2_loop)

    lea rax, [rcx + r10 + 32] ; return match position
    sub rax, r9
    vzeroupper
    ret

L(cross_page_vec2_loop_end):
    sub rcx, r9
    lea rax, [r9 + 32*3 - 1]
    cmp rdx, rax
    jbe L(cross_page_vec_last)

    mov QWORD PTR [0123h], rax

    align 16
L(cross_page_vec_last):
    lea rcx, [rcx + rdx - 32]
    neg r9
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + r9 + 1]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx]
    lea rcx, [rcx + r9 + 1]
    sub rcx, r9

    and rdx, 32-1
    not rdx

    mov eax, -1
    shlx r10d, eax, edx

    vpand ymm4, ymm2, ymm3

    vpmovmskb eax, ymm4
    and eax, r10d ; filter out already checked characters
L(cross_page_vec_last_loop):
    test eax, eax
    jz L(cross_page_vec_last_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10 - 32]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(cross_page_vec_last_loop)

    lea rax, [rcx + r10] ; return match position
    add rax, r9
    vzeroupper
    ret

L(cross_page_vec_last_loop_end):
    xor rax, rax
    vzeroupper
    ret

