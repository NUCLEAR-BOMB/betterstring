
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

.code

; const char* string (rcx) - pointer to string to compare
; size_t      count (rdx) - lenght of the string
; const char* needle (r8) - pointer to character sequence
; size_t      needle_size (r9) - lenght of the character sequence
;
; Finds the position of first character that equal to one of the characters in the character sequence.
;
; Note that this function uses AVX2 and BMI2 processor extensions.
betterstring_strfirst_of_avx2 PROC

IF 1

    test rdx, rdx
    jz cmp_0

    cmp r9, 5
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

    align 16
cmp_0:
    xor rax, rax
    ret

    align 16
cmp_1:
    include strfirst_of/cmp_1.asm

    align 16
cmp_2:
    include strfirst_of/cmp_2.asm

    align 16
cmp_3:
    include strfirst_of/cmp_3.asm

    align 16
cmp_4:
    include strfirst_of/cmp_4.asm

    align 16
cmp_5:
    include strfirst_of/cmp_5.asm

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

    mov rax, 0FFh

    align 16
cmp_bitmap_mark_loop:
    movzx r10, BYTE PTR [r8 + r9 - 1]
    mov BYTE PTR [rsp + r10], al
    dec r9
    jnz cmp_bitmap_mark_loop
    ; r9 is zero

    vzeroupper

    ; use r9 as counter register
    align 16
cmp_bitmap_loop:
    movzx r10, BYTE PTR [rcx + r9]

    test al, BYTE PTR [rsp + r10]
    jnz cmp_bitmap_return

    inc r9
    cmp r9, rdx
    jne cmp_bitmap_loop

    add rsp, 256

    xor rax, rax
    ret

    align 16
cmp_bitmap_return:
    add rsp, 256

    lea rax, [rcx + r9]
    ret

ELSE

    test rdx, rdx
    jz cmp_0
    test r9, r9
    jz cmp_0

    cmp r9, 1
    je cmp_1

    include strfirst_of/cmp_any.asm

    align 16
cmp_0:
    xor rax, rax
    ret

    align 16
cmp_1:
    include strfirst_of/cmp_1.asm

ENDIF

betterstring_strfirst_of_avx2 ENDP


END
