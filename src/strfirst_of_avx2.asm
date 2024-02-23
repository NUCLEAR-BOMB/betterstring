
PAGE_SIZE equ 4096

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

    test rdx, rdx
    jz cmp_0

    cmp r9, 4
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

    align 16
cmp_0:
    xor rax, rax
    ret

include strfirst_of/cmp_1.asm

include strfirst_of/cmp_2.asm

include strfirst_of/cmp_3.asm

include strfirst_of/cmp_4.asm

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

    xor r10, r10
    mov al, 0FFh
    align 16
cmp_bitmap_mark_loop:
    mov r10b, BYTE PTR [r8 + r9 - 1]
    mov BYTE PTR [rsp + r10], al
    dec r9
    jnz cmp_bitmap_mark_loop

    xor r9, r9
    xor r11, r11
    align 16
cmp_bitmap_loop:
    mov r10b, BYTE PTR [rcx + r11]
    cmp r9b, BYTE PTR [rsp + r10]
    jne cmp_bitmap_return
    inc r11
    cmp r11, rdx
    jne cmp_bitmap_loop

    add rsp, 256
    xor rax, rax
    vzeroupper
    ret

    align 16
cmp_bitmap_return:
    add rsp, 256
    lea rax, [rcx + r11]
    vzeroupper
    ret

betterstring_strfirst_of_avx2 ENDP


END
