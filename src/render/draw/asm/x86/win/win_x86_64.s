.intel_syntax noprefix

.extern g_pVidbuf
.extern g_pTex

.section .text
    .globl D_VertLine # D_VertLine(int y0, int y1, int x, u32 color)
        D_VertLine:
            mov r10d, edx

            # ecx  -> y0
            # r10d -> y1
            # r8d  -> x
            # r9d  -> color

            .Loop:
                cmp ecx, r10d # if (counter > y1)
                jg .Exit      #    goto .Exit

                mov  eax, ecx # \
                imul eax, 384 #  | eax = y * SCREEN_WIDTH + x 
                mov  edx, r8d #  |      
                add  eax, edx # /
                cdqe # eax -> rax

                lea rdx, [rax*4]   # \
                mov rax, g_pVidbuf #  | rdx = buf[y * SCREEN_WIDTH + x]
                add rdx, rax       # /

                mov eax, r9d             # \
                                         #  | buf[y * SCREEN_WIDTH + x] = color
                mov DWORD PTR [rdx], eax # /

                inc ecx    # counter++
                jmp .Loop  # goto .Loop

            .Exit: # exit from loop

            ret