
OPTION AVXENCODING: PREFER_VEX

.code

; string: rcx PTR BYTE
; lenght: rdx QWORD
; char: r8b   BYTE
betterstring_impl_strrfind_ch_avx2 PROC
        ;int 3
    add rcx, rdx
    xor rax, rax
    cmp rdx, 32
    jb fallback ; lenght < 32
    mov rax, rcx
    and rax, 32-1
    jz general
    sub rdx, rax
    sub rcx, rax
    dec rcx
align_loop:
    cmp r8b, BYTE PTR [rcx + rax]
    je exit
    dec rax
    jnz align_loop ; rax == 0

    inc rcx
    cmp rdx, 32
    jb fallback ; lenght < 32
general:
    movzx r8d, r8b
    vmovd xmm0, r8d
    vpbroadcastb ymm0, xmm0
general_loop:
    sub rcx, 32
    vmovdqa ymm1, YMMWORD PTR [rcx]
    vpcmpeqb ymm1, ymm1, ymm0
    vpmovmskb rax, ymm1
    bsr rax, rax ; BitScanReverse
    jnz exit ; rax == 0
    sub rdx, 32
    cmp rdx, 32
    jae general_loop ; rdx >= 32
fallback:
    test rdx, rdx
    jz not_found_exit ; lenght == 0
fallback_loop:
    dec rcx
    cmp r8b, BYTE PTR [rcx]
    je exit
    dec rdx
    jne fallback_loop
not_found_exit:
    vzeroupper
    ret                     ; return 0
exit:
    add rax, rcx
    vzeroupper
    ret
betterstring_impl_strrfind_ch_avx2 ENDP

END
