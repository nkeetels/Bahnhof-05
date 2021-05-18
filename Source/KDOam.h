// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDOam.h
// Description: Handles the initialization and rendering of sprites. 
//
// Note: This probably originas from TONC

#ifndef __KDOAM_H
#define __KDOAM_H

#include "KDTypes.h"

// Attribute 0 
#define DOUBLESIZE 0x0200
#define TRANSPARENT 0x0400
#define COLOR16 0x0000
#define COLOR256 0x2000
#define SQUARE 0x0000
#define WIDE 0x4000
#define TALL 0x8000
#define AFFINE_NORMAL 0x0100
#define AFFINE_DOUBLE 0x0300

// Attribute 1
#define HFLIP 0x1000
#define VFLIP 0x2000
#define SIZE_8 0x0000
#define SIZE_16 0x4000
#define SIZE_32 0x8000
#define SIZE_64 0xC000

// Attribute 2
#define PRIORITY_MASK 0x0C00
#define PRIORITY(n) ((n)<<10)

typedef struct OAMENTRY
{
  u16 attribute0;
  u16 attribute1;
  u16 attribute2;
  u16 attribute3;
}OAM_Entry;

typedef struct AFFINE_ENTRY 
{
   u16 filler1[3];
   u16 pa;
   u16 filler2[3];
   u16 pb;	
   u16 filler3[3];
   u16 pc;	
   u16 filler4[3];
   u16 pd;
} AFFINE_Entry;

#define MEM_OAM (OAM_Entry*)0x07000000

// Sprites on top, BG2 on bottom, invsrc
#define ALPHA_BLEND (0x10 | 0x0400 | 0x40)
#define BLEND(white, black) (white+(black<<8))

extern OAM_Entry OAM_Entries[128]; 

void OAM_SetBlendMode(u16 mode, u16 value);
void OAM_CopySprites();
void OAM_SetSprite(u8 id, int xpos, int ypos, u8 size, u8 rotate, u8 transparent, const u16 *image, const u16 *palette);
void OAM_RotateSprite(u8 id, u8 angle, int xscale, int yscale);
void OAM_Clear();
void OAM_Overlay(const u16 *palette, const u16 *image1, const u16 *image2, const u16 *image3, const u16 *image4);

#endif//__KDOAM_H
