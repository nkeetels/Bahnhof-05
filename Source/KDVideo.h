// Bahnhof-05 Gameboy Advance demo source code
// 
// File: KDVideo.h
// Description: GBA constants related to video output

#ifndef __KDVIDEO_H
#define __KDVIDEO_H

#include "KDTypes.h"

#define SCREENWIDTH 240
#define SCREENHEIGHT 160
#define SCREENPIXELS SCREENWIDTH * SCREENHEIGHT
#define HALFWIDTH SCREENWIDTH / 2
#define HALFHEIGHT SCREENHEIGHT / 2

#define MEM_TILES (CHARBLOCK*)0x06000000
#define MEM_OBJPAL (u16*)0x5000200
#define MEM_OBJTILES (u16*)0x6010000
#define MEM_BGPAL (u16*)0x5000000
#define MEM_BGTILES (u16*)0x6004000

#define FRONTBUFFER (u16*)0x6000000
#define BACKBUFFER (u16*)0x600A000

#define RGB16(r,g,b) ((r)+((g)<<5)+((b)<<10))
#define RGB15(r,g,b) ((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10)

#endif//__KDVIDEO_H
