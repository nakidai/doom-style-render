.intel_syntax noprefix

.section .text
    pi:  .float 3.1415
    tau: .float 6.283

    .globl R_NormalizeAngle
        R_NormalizeAngle: # f32 R_NormalizeAngle(f32 a)
            movaps xmm1, xmm0

            addps xmm0, pi  # \    
                            #  | (a + PI) / TAU
            divps xmm0, tau # / 

            cvttsd2si eax, xmm0 # \
                                #  | (int) ((a + PI) / TAU)
            cvtsi2ss  xmm0, eax # /

            mulps xmm0, [tau] # \
                              #  | a - (TAU * (int) ((a + PI) / TAU))
            subps xmm1, xmm0  # /
            
            movaps xmm0, xmm1
            ret

    .globl R_AbgrMul
        R_AbgrMul:
            ret

    .globl R_Rotate
        R_Rotate:
            ret