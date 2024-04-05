
L MACRO label_name:REQ
    EXITM <@CatStr(<vec_needle_>, label_name)>
ENDM

    ; r9 (needle length) <= 32
    ; r9 (needle length) <= rdx (haystack length)

    mov rax, r8
    and rax, PAGE_SIZE-1
    cmp rax, PAGE_SIZE-32
    ja L(needle_cross_page)

    vmovdqu ymm5, YMMWORD PTR [r8]
L(needle_cross_page_continue):

; ===================== LOOP 1 PROLOGUE =====================

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
    lea rax, [r9 + 32*2 - 1]
    cmp rdx, rax
    jbe L(vec_last)

; ===================== LOOP 2 PROLOGUE =====================

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
    lea rax, [r9 + 32*3 - 1]
    cmp rdx, rax
    jbe L(vec_last)

; ===================== LOOP 3 PROLOGUE =====================

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*2]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1 + 32*2]
    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4
L(vec3_loop):
    test eax, eax
    jz L(vec3_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10 + 32*2]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(vec3_loop)

    lea rax, [rcx + r10 + 32*2] ; return match position
    vzeroupper
    ret

L(vec3_loop_end):
    lea rax, [r9 + 32*4 - 1]
    cmp rdx, rax
    jbe L(vec_last)

; ===================== LOOP 4 PROLOGUE =====================

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + 32*3]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx + r9 - 1 + 32*3]
    vpand ymm4, ymm2, ymm3
    vpmovmskb eax, ymm4
L(vec4_loop):
    test eax, eax
    jz L(vec4_loop_end)

    xor r10d, r10d
    tzcnt r10d, eax

    vpcmpeqb ymm2, ymm5, YMMWORD PTR [rcx + r10 + 32*3]
    btr eax, r10d

    vpmovmskb r11d, ymm2
    andn r11d, r11d, r8d
    jnz L(vec4_loop)

    lea rax, [rcx + r10 + 32*3] ; return match position
    vzeroupper
    ret

L(vec4_loop_end):
    lea rax, [r9 + 32*5 - 1]
    cmp rdx, rax
    jbe L(vec_last)

    mov QWORD PTR [01234h], rax

    align 16
L(vec_last):
    ; int 3

    lea rcx, [rcx + rdx - 32]
    neg r9
    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rcx + r9 + 1]
    vpcmpeqb ymm3, ymm1, YMMWORD PTR [rcx]
    lea rcx, [rcx + r9 + 1]

    add rdx, r9
    and rdx, 32-1
    not rdx

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

; _RDATA SEGMENT ALIGN(64) ALIAS(".rdata") READONLY
;     align 64
; L(needle_shuffle_mask):
;     DB 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
; 
; _RDATA ENDS

    align 16
L(needle_cross_page):
    sub rsp, 64
    ; failed store forwarding
    vmovdqu ymm5, YMMWORD PTR [r8 + r9 - 32]
    vmovdqu YMMWORD PTR [rsp + r9], ymm5
    vmovdqu ymm5, YMMWORD PTR [rsp + 32]
    add rsp, 64
    jmp L(needle_cross_page_continue)
