
#include "hw.h"

/* General internal/io stuff */

#define RI_P1    0x00
#define RI_SB    0x01
#define RI_SC    0x02
#define RI_DIV   0x04
#define RI_TIMA  0x05
#define RI_TMA   0x06
#define RI_TAC   0x07

#define RI_KEY1  0x4D

#define RI_RP    0x56

#define RI_SVBK  0x70



/* Interrupts flags */

#define RI_IF    0x0F
#define RI_IE    0xFF




/* LCDC */

#define RI_LCDC  0x40
#define RI_STAT  0x41
#define RI_SCY   0x42
#define RI_SCX   0x43
#define RI_LY    0x44
#define RI_LYC   0x45
#define RI_DMA   0x46
#define RI_BGP   0x47
#define RI_OBP0  0x48
#define RI_OBP1  0x49
#define RI_WY    0x4A
#define RI_WX    0x4B

#define RI_VBK   0x4F

#define RI_HDMA1 0x51
#define RI_HDMA2 0x52
#define RI_HDMA3 0x53
#define RI_HDMA4 0x54
#define RI_HDMA5 0x55

#define RI_BCPS  0x68
#define RI_BCPD  0x69
#define RI_OCPS  0x6A
#define RI_OCPD  0x6B



#define REG(n) hw.regs[(n)]



/* General internal/io stuff */

#define R_P1    REG(RI_P1)
#define R_SB    REG(RI_SB)
#define R_SC    REG(RI_SC)
#define R_DIV   REG(RI_DIV)
#define R_TIMA  REG(RI_TIMA)
#define R_TMA   REG(RI_TMA)
#define R_TAC   REG(RI_TAC)

#define R_KEY1  REG(RI_KEY1)

#define R_RP    REG(RI_RP)

#define R_SVBK  REG(RI_SVBK)



/* Interrupts flags */

#define R_IF    REG(RI_IF)
#define R_IE    REG(RI_IE)




/* LCDC */

#define R_LCDC  REG(RI_LCDC)
#define R_STAT  REG(RI_STAT)
#define R_SCY   REG(RI_SCY)
#define R_SCX   REG(RI_SCX)
#define R_LY    REG(RI_LY)
#define R_LYC   REG(RI_LYC)
#define R_DMA   REG(RI_DMA)
#define R_BGP   REG(RI_BGP)
#define R_OBP0  REG(RI_OBP0)
#define R_OBP1  REG(RI_OBP1)
#define R_WY    REG(RI_WY)
#define R_WX    REG(RI_WX)

#define R_VBK   REG(RI_VBK)

#define R_HDMA1 REG(RI_HDMA1)
#define R_HDMA2 REG(RI_HDMA2)
#define R_HDMA3 REG(RI_HDMA3)
#define R_HDMA4 REG(RI_HDMA4)
#define R_HDMA5 REG(RI_HDMA5)

#define R_BCPS  REG(RI_BCPS)
#define R_BCPD  REG(RI_BCPD)
#define R_OCPS  REG(RI_OCPS)
#define R_OCPD  REG(RI_OCPD)







