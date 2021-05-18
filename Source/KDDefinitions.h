
#ifndef __KDDEFINITIONS_H
#define __KDDEFINITIONS_H

#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))

//#define ALIGN(n) __attribute__((aligned(n)))

#define MODE_0 0x0000
#define MODE_1 0x0001
#define MODE_2 0x0002
#define MODE_3 0x0003
#define MODE_4 0x0004
#define MODE_5 0x0005
#define OBJ_2D 0x0000
#define OBJ_1D 0x0040
#define FORCE_BLANK 0x0080
#define ENABLE_BG0 0x0100
#define ENABLE_BG1 0x0200
#define ENABLE_BG2 0x0400
#define ENABLE_BG3 0x0800
#define ENABLE_OBJ 0x1000

#endif//__KDDEFINITIONS_H
