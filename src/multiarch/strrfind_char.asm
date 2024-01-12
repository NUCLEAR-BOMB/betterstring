

OPTION AVXENCODING: PREFER_VEX

.code

; Returns a pointer to the last matching character.
; The search starts at the end of the string.

; This function uses the AVX2 processor extension quickly process 32 bytes at a time,
; but because of this, this function also requires to be CPU dispatched.

; string: rcx PTR BYTE
; lenght: rdx QWORD
; char: r8b   BYTE

betterstring_impl_strrfind_ch_avx2 PROC
    add rcx, rdx                    ; Advance the string pointer to the end
    xor rax, rax                    ; Set the return value to 0 (nullptr)
    cmp rdx, 32
    jb fallback                     ; If the string is too small for a 32 byte search
    ; cmp rdx, 100
    ; jbe general
    mov rax, rcx
    and rax, 32-1                   ; Align the pointer to a 32 byte boundary
    jz general                      ; If the pointer is already aligned
    sub rdx, rax                    ; Substract from the length a misaligned pointer offset
    sub rcx, rax                    ; Substract from the string pointer a misaligned pointer offset
                                    ; so rcx + rax = 0
    dec rcx                         ; Start iterating from the last character, but not from the out of bounds one
align_loop:                         ; Align the string pointer to a 32 byte boundary
    cmp r8b, BYTE PTR [rcx + rax]   
    je exit                         ; If the character has already been found
    dec rax                         ; Advance backwards to the next character
    jnz align_loop                  ; If there are still characters left
                                    ; The string pointer is aligned to a 32 byte boundary
    inc rcx
    cmp rdx, 32
    jb fallback ; lenght < 32       ; If the string is too small for a 32 byte search after alignment
general:
    movzx r8d, r8b
    vmovd xmm0, r8d                 ; Fill the ymm0 with the search character
    vpbroadcastb ymm0, xmm0         ; _mm256_set1_epi8(r8b)
general_loop:
    sub rcx, 32                     ; Advance the string pointer so that we can read next 32 bytes
    prefetchnta [rcx - 1024]
    vmovdqa ymm1, YMMWORD PTR [rcx] ; Benchmarks show that doing 'vmovdqa' is faster than directly using 'vpcmpeqb'
    vpcmpeqb ymm1, ymm1, ymm0       ; Compare the loaded 32 bytes with the search character
    vpmovmskb rax, ymm1             ; Minimize the comparison result in the 4 byte register
    bsr rax, rax                    ; Get position of the last matching character
    jnz exit                        ; If there is at least one corresponding character
    sub rdx, 32                     ; Decrease the remaining string lenght
    cmp rdx, 32
    jae general_loop                ; If there is still a 32 byte string left
fallback:                           ; Simple loop that checks the remaining string
    test rdx, rdx
    jz not_found_exit               ; If the string lenght is 0
fallback_loop:
    dec rcx                         ; Advance the string pointer to the next not checked character
    cmp r8b, BYTE PTR [rcx]
    je exit                         ; If the character has been found
    dec rdx                         ; Decrement the string lenght
    jne fallback_loop               ; If the string lenght is not equal to 0
not_found_exit:
    vzeroupper
    ret                             ; Returns 0 (nullptr). 'rax' should be equal to 0
exit:
    add rax, rcx
    ; The above instruction can be executed from several places in the program: align_loop, general_loop, fallback_loop.
    ;
    ; First, let's assume was taken path from align_loop.
    ; Before the jump instruction, we compare the search character with an address rcx + rax,
    ; so if we ended up here the last matched character will be just right rcx + rax ('add rax, rcx')
    ;
    ; Secondly, let's assume that was taken path from general_loop.
    ; Before we got here, there was called 'bsr' instruction that set rax register to the last matched index,
    ; and rcx register that was set to the 32 byte aligned chunk.
    ; The address of this found character can be calculated using 'rax + rcx' which is what we are doing here.
    ;
    ; Thirdly, let's assume that was taken path from fallback_loop.
    ; The last comparison was with rcx register, and it is our character we are looking for.
    ; But what about rax register? It will always be set to 0, because for general_loop and align_loop
    ; they can be exited into fallback_loop when rax is 0.
    ; (in first scenario 'bsr' will return 0, in second scenario loop can be only exited when rax is 0).
    vzeroupper
    ret
betterstring_impl_strrfind_ch_avx2 ENDP

END
