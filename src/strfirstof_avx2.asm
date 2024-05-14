
; // Copyright 2024.
; // Distributed under the Boost Software License, Version 1.0.
; // (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

PAGE_SIZE equ 4096

PUSH_XMM MACRO regs:VARARG
    LOCAL count
    count = 0
    FOR reg,<regs>
        count = count + 1
    ENDM

    sub rsp, (16*count)

    count = 0
    FOR reg,<regs>
        vmovdqu XMMWORD PTR [rsp + (16*count)], reg
        count = count + 1
    ENDM
ENDM

POP_XMM MACRO regs:VARARG
    LOCAL count
    count = 0
    FOR reg,<regs>
        vmovdqu reg, XMMWORD PTR [rsp + (16*count)]
        count = count + 1
    ENDM
    add rsp, (16*count)
ENDM

MM256_SET1_EPI8 MACRO out_reg:REQ, location:REQ
    ; LOCAL tmp_xmm_reg
    ; tmp_xmm_reg TEXTEQU @CatStr(xmm, @SubStr(out_reg, 4, 1))

    ; movzx r9d, location
    ; vmovd tmp_xmm_reg, r9d

    vpbroadcastb out_reg, location
ENDM

; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence
;
; Finds the position of first character that equal to one of the characters in the character sequence.
;
; Note that this function uses AVX2 and BMI2 processor extensions.

; Use COFF's feature grouped sections to define text section with 64 byte alignment
_TEXT$align64 SEGMENT ALIGN(64)
    align 64
betterstring_strfirstof_avx2 PROC

IF 1
    test rdx, rdx
    jz cmp_0

    cmp r9, 6
    ja cmp_bitmap

    lea r10, [cmp_jump_table]
    movsxd rax, DWORD PTR [r10 + r9*4]
    add rax, r10
    jmp rax

cmp_jump_table:
    dd cmp_0-cmp_jump_table
    dd cmp_1-cmp_jump_table
    dd cmp_2-cmp_jump_table
    dd cmp_3-cmp_jump_table
    dd cmp_4-cmp_jump_table
    dd cmp_5-cmp_jump_table
    dd cmp_6-cmp_jump_table

    align 64
cmp_0:
    xor rax, rax
    ret

    align 64
cmp_1:
    include strfirstof/cmp_1.asm

    align 64
cmp_2:
    include strfirstof/cmp_2.asm

    align 64
cmp_3:
    include strfirstof/cmp_3.asm

    align 64
cmp_4:
    include strfirstof/cmp_4.asm

    align 64
cmp_5:
    include strfirstof/cmp_5.asm

    align 64
cmp_6:
    include strfirstof/cmp_6.asm

    align 64
cmp_bitmap:
    sub rsp, 256

    vpxor ymm0, ymm0, ymm0
    vmovdqu YMMWORD PTR [rsp + 32*0], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*1], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*2], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*3], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*4], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*5], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*6], ymm0
    vmovdqu YMMWORD PTR [rsp + 32*7], ymm0

    mov rax, rcx
    mov rcx, 0FFh

    align 16
cmp_bitmap_mark_loop:
    movzx r10, BYTE PTR [r8 + r9 - 1]
    mov BYTE PTR [rsp + r10], cl

    dec r9
    jnz cmp_bitmap_mark_loop

    test rdx, 1
    jz cmp_bitmap_loop

    movzx r11, BYTE PTR [rax]
    test BYTE PTR [rsp + r11], cl
    jnz cmp_bitmap_return_0

    inc rax
    dec rdx
    jz cmp_bitmap_nullptr

    align 64
cmp_bitmap_loop:
    movzx r10, BYTE PTR [rax]
    test BYTE PTR [rsp + r10], cl
    jnz cmp_bitmap_return_0

    movzx r11, BYTE PTR [rax + 1]
    test BYTE PTR [rsp + r11], cl
    jnz cmp_bitmap_return_1

    add rax, 2
    sub rdx, 2
    jnz cmp_bitmap_loop

cmp_bitmap_nullptr:
    vzeroupper
    add rsp, 256
    xor eax, eax
    ret

    align 16
cmp_bitmap_return_0:
    add rsp, 256
    vzeroupper
    ret

    align 16
cmp_bitmap_return_1:
    add rsp, 256
    inc rax
    vzeroupper
    ret

    align 16
cmp_bitmap_return_n1:
    add rsp, 256
    dec rax
    vzeroupper
    ret

ELSE

    test rdx, rdx
    jz cmp_0
    test r9, r9
    jz cmp_0

    cmp r9, 1
    je cmp_1

    include strfirstof/cmp_any.asm

    align 16
cmp_0:
    xor rax, rax
    ret

    align 16
cmp_1:
    include strfirstof/cmp_1.asm

ENDIF

betterstring_strfirstof_avx2 ENDP

_TEXT$align64 ENDS

END
