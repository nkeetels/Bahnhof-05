// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDRegs.h
// Description: Defines Gameboy Advance hardware registers.
// 
// Note: DevkitPro probably has a header file for these.

#ifndef __KDREGS_H
#define __KDREGS_H

#include "KDTypes.h"

#define REG_DISPCNT *(volatile u32*)0x04000000
#define REG_DISPSTAT *(volatile u16*)0x04000004
#define REG_VCOUNT *(volatile u16*)0x04000006
#define REG_BGCNT (volatile u16*)0x04000008
#define REG_BG0CNT *(volatile u16*)0x04000008
#define REG_BG1CNT *(volatile u16*)0x0400000A
#define REG_BG2CNT *(volatile u16*)0x0400000C
#define REG_BG3CNT *(volatile u16*)0x0400000E
#define REG_BG0HOFS (volatile u16*)0x04000010
#define REG_BG0VOFS *(volatile u16*)0x04000012
#define REG_BG1HOFS *(volatile u16*)0x04000014
#define REG_BG1VOFS *(volatile u16*)0x04000016
#define REG_BG2HOFS *(volatile u16*)0x04000018
#define REG_BG2VOFS *(volatile u16*)0x0400001A
#define REG_BG3HOFS *(volatile u16*)0x0400001C
#define REG_BG3VOFS *(volatile u16*)0x0400001E
#define REG_BG2PA *(volatile s16*)0x04000020
#define REG_BG2PB *(volatile s16*)0x04000022
#define REG_BG2PC *(volatile s16*)0x04000024
#define REG_BG2PD *(volatile s16*)0x04000026
#define REG_BG2X *(volatile s32*)0x04000028
#define REG_BG2Y *(volatile s32*)0x0400002C
#define REG_BG3PA *(volatile s16*)0x04000030
#define REG_BG3PB *(volatile s16*)0x04000032
#define REG_BG3PC *(volatile s16*)0x04000034
#define REG_BG3PD *(volatile s16*)0x04000036
#define REG_BG3X *(volatile s32*)0x04000038
#define REG_BG3Y *(volatile s32*)0x0400003C
#define REG_BLDCNT *(volatile u16*)0x04000050
#define REG_BLDALPHA *(volatile u16*)0x04000052
#define REG_BLDY *(volatile u16*)0x04000054

#endif//__KDREGS_H
