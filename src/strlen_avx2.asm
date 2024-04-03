
; // Copyright 2024.
; // Distributed under the Boost Software License, Version 1.0.
; // (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

OPTION AVXENCODING:PREFER_VEX

PAGE_SIZE equ 1 SHL 12 ; 4096

.code

; const char* string (rcx) - pointer to null-terminated string to be examined
; returns: size_t (rax) - lenght of the null-terminated string

betterstring_strlen_avx2 PROC
    vpxor ymm0, ymm0, ymm0

    mov rdx, rcx
    and rdx, PAGE_SIZE - 1
    cmp rdx, PAGE_SIZE - 32
    ja cross_page           ; if next 32 bytes cross the page boundary, i.e. cannot be loaded into YMM

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rcx]
    vpmovmskb rdx, ymm1
    bsf rax, rdx
    jz align_vec8

    vzeroupper
    ret

    align 16
align_vec8:
    ; int 3

    mov rdx, rcx
    and rdx, -32            ; align downwards

    mov r9, rcx
    or r9, 32*8-1
    sub r9, rdx

    ; cmp r9, 32*8-1
    ; je vec8_loop

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rdx + 32*1]
    vpmovmskb r8, ymm2
    bsf r8, r8
    jnz return_vec1
    cmp r9, 32*1-1
    je vec8_loop_prol

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rdx + 32*2]
    vpmovmskb r8, ymm3
    bsf r8, r8
    jnz return_vec2
    cmp r9, 32*2-1
    je vec8_loop_prol

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rdx + 32*3]
    vpmovmskb r8, ymm4
    bsf r8, r8
    jnz return_vec3
    cmp r9, 32*3-1
    je vec8_loop_prol

    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rdx + 32*4]
    vpmovmskb r8, ymm5
    bsf r8, r8
    jnz return_vec4
    cmp r9, 32*4-1
    je vec8_loop_prol

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rdx + 32*5]
    vpmovmskb r8, ymm1
    bsf r8, r8
    jnz return_vec5
    cmp r9, 32*5-1
    je vec8_loop_prol

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rdx + 32*6]
    vpmovmskb r8, ymm2
    bsf r8, r8
    jnz return_vec6
    cmp r9, 32*6-1
    je vec8_loop_prol

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rdx + 32*7]
    vpmovmskb r8, ymm3
    bsf r8, r8
    jnz return_vec7
    cmp r9, 32*7-1
    je vec8_loop_prol

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rdx + 32*8]
    vpmovmskb r8, ymm4
    bsf r8, r8
    jnz return_vec8

    jmp vec8_loop_prol

    align 16
return_vec1:
    lea rax, [rdx + r8 + 32*1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec2:
    lea rax, [rdx + r8 + 32*2]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec3:
    lea rax, [rdx + r8 + 32*3]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec4:
    lea rax, [rdx + r8 + 32*4]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec5:
    lea rax, [rdx + r8 + 32*5]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec6:
    lea rax, [rdx + r8 + 32*6]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec7:
    lea rax, [rdx + r8 + 32*7]
    sub rax, rcx
    vzeroupper
    ret

    align 16
return_vec8:
    lea rax, [rdx + r8 + 32*8]
    sub rax, rcx
    vzeroupper
    ret

    ; align 16
vec8_loop_prol:
    or rdx, 32*8-1

    align 16
vec8_loop:
    vmovdqa ymm1,       YMMWORD PTR [rdx + 32*0 + 1]
    vpminub ymm2, ymm1, YMMWORD PTR [rdx + 32*1 + 1]
    vmovdqa ymm3,       YMMWORD PTR [rdx + 32*2 + 1]
    vpminub ymm4, ymm3, YMMWORD PTR [rdx + 32*3 + 1]
    vpminub ymm5, ymm2, ymm4

    vmovdqa ymm1,       YMMWORD PTR [rdx + 32*4 + 1]
    vpminub ymm2, ymm1, YMMWORD PTR [rdx + 32*5 + 1]
    vmovdqa ymm3,       YMMWORD PTR [rdx + 32*6 + 1]
    vpminub ymm4, ymm3, YMMWORD PTR [rdx + 32*7 + 1]
    vpminub ymm1, ymm2, ymm4

    vpminub ymm1, ymm1, ymm5

    ; ymm0 is filled with zeros
    vpcmpeqb ymm1, ymm0, ymm1

    add rdx, 32*8
    vptest ymm1, ymm1
    jz vec8_loop

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rdx - 32*8 + 1]
    vpmovmskb r8, ymm1
    bsf r8, r8
    jnz vec8_loop_return_vec1

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rdx - 32*7 + 1]
    vpmovmskb r8, ymm2
    bsf r8, r8
    jnz vec8_loop_return_vec2

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rdx - 32*6 + 1]
    vpmovmskb r8, ymm3
    bsf r8, r8
    jnz vec8_loop_return_vec3

    vpcmpeqb ymm4, ymm0, YMMWORD PTR [rdx - 32*5 + 1]
    vpmovmskb r8, ymm4
    bsf r8, r8
    jnz vec8_loop_return_vec4

    vpcmpeqb ymm5, ymm0, YMMWORD PTR [rdx - 32*4 + 1]
    vpmovmskb r8, ymm5
    bsf r8, r8
    jnz vec8_loop_return_vec5

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rdx - 32*3 + 1]
    vpmovmskb r8, ymm1
    bsf r8, r8
    jnz vec8_loop_return_vec6

    vpcmpeqb ymm2, ymm0, YMMWORD PTR [rdx - 32*2 + 1]
    vpmovmskb r8, ymm2
    bsf r8, r8
    jnz vec8_loop_return_vec7

    vpcmpeqb ymm3, ymm0, YMMWORD PTR [rdx - 32*1 + 1]
    vpmovmskb r8, ymm3
    bsf r8, r8
    jmp vec8_loop_return_vec8

    align 16
vec8_loop_return_vec1:
    lea rax, [rdx + r8 - 32*8 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec2:
    lea rax, [rdx + r8 - 32*7 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec3:
    lea rax, [rdx + r8 - 32*6 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec4:
    lea rax, [rdx + r8 - 32*5 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec5:
    lea rax, [rdx + r8 - 32*4 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec6:
    lea rax, [rdx + r8 - 32*3 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec7:
    lea rax, [rdx + r8 - 32*2 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
vec8_loop_return_vec8:
    lea rax, [rdx + r8 - 32*1 + 1]
    sub rax, rcx
    vzeroupper
    ret

    align 16
cross_page:
    mov rdx, rcx
    and rdx, -32

    vpcmpeqb ymm1, ymm0, YMMWORD PTR [rdx]
    vpmovmskb r8, ymm1

    mov r9, rcx
    and r9, 32-1
    shrx r8d, r8d, r9d

    bsf rax, r8
    jz vec8_loop_prol

    vzeroupper
    ret

betterstring_strlen_avx2 ENDP

END
