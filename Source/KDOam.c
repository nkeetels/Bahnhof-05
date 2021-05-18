// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDOam.c
// Description: Handles the initialization and rendering of sprites. 
//
// Note: This probably originates from TONC

#include "KDEngine.h"
#include "KDRegs.h"
#include "KDOam.h"

OAM_Entry OAM_Entries[128]; 
AFFINE_Entry *Affine_Entries = (AFFINE_Entry*)OAM_Entries;
u32 spriteAddr=0;
u32 tileAddr=0;

void OAM_SetBlendMode(u16 mode, u16 value)
{
  REG_BLDCNT = mode;
  REG_BLDALPHA = value;
}

void OAM_CopySprites()
{
  u16 *sprites = (u16*)OAM_Entries;
  u16 *oam = MEM_OAM;
  Memcpy32(oam, sprites, 512);	
}

void OAM_SetSprite(u8 id, int xpos, int ypos, u8 size, u8 rotate, u8 transparent, const u16 *image, const u16 *palette)
{
	int att0=0;
	int att1=0;
	
	if(size==64)
		att1|=SIZE_64;
	else if(size==32)
		att1|=SIZE_32;
	else if(size==16)
		att1|=SIZE_16;
	else if(size==8)
		att1|=SIZE_8;

	Affine_Entries[id].pa = 137;
	Affine_Entries[id].pd = 204;
	
	att0|=ypos;
	att0|=COLOR256|SQUARE;
	
	att1|=xpos;
	if(rotate)
	{
		att0|=DOUBLESIZE;
		att0|=AFFINE_NORMAL;
		att1|=id << 9;
	}

	if(transparent) 
	{
		att0|=TRANSPARENT;
	}
	
	OAM_Entries[id].attribute0=att0;
	OAM_Entries[id].attribute1=att1;
	OAM_Entries[id].attribute2=512 + spriteAddr;

	if(size==64)
		spriteAddr+=128;
	else if(size==32)
		spriteAddr+=32;
	else if(size==16)
		spriteAddr+=8;
	else if(size==8)
		spriteAddr+=2;
	
	u32 *addr=(u32)0x601C000+(u32)tileAddr;
	DMACopy((u32*)palette, (u32*)0x5000200,128,0x84000000);
	DMACopy(image, addr, (size*size)>>2,0x84000000);
	
	tileAddr+=(size*size);
}

void OAM_Clear()
{
	int i;
	for (i = 0; i < 128; i++)
	{
		OAM_Entries[i].attribute0 = 0;
		OAM_Entries[i].attribute1 = 0;
		OAM_Entries[i].attribute2 = 0;
		OAM_Entries[i].attribute3 = 0;
	}
	spriteAddr = 0;
	tileAddr = 0;
}

void OAM_RotateSprite(u8 id, u8 angle, int xscale, int yscale)
{
	Affine_Entries[id].pa = (xscale * SinLUT[(angle+64)&255]) >> 8;
	Affine_Entries[id].pb = (yscale * SinLUT[angle&255]) >> 8;
	Affine_Entries[id].pc = ((-xscale) * SinLUT[angle&255]) >> 8;
	Affine_Entries[id].pd = (yscale * SinLUT[(angle+64)&255]) >> 8;
}

void OAM_SetAttributes(u8 id, u16 attrib0, u16 attrib1, u16 attrib2, u16 attrib3)
{
	OAM_Entries[id].attribute0 = attrib0;
	OAM_Entries[id].attribute1 = attrib1;
	OAM_Entries[id].attribute2 = attrib2;
	OAM_Entries[id].attribute3 = attrib3;
}

void OAM_Overlay(const u16 *palette, const u16 *image1, const u16 *image2, const u16 *image3, const u16 *image4)
{
	OAM_Clear();

	FastCopy16(0x5000200, palette, 256);

	FastCopy16((u16*)0x601C000, image1, 2048);
	FastCopy16((u16*)0x601D000, image2, 2048);
	FastCopy16((u16*)0x601E000, image3, 2048);
	FastCopy16((u16*)0x601F000, image4, 2048);

	OAM_SetAttributes(0, 0x27E7, 0xC1FB, 0x0200, 0);
	OAM_SetAttributes(1, 0x27E7, 0xC273, 0x0280, 0);
	OAM_SetAttributes(2, 0x2738, 0xC5FB, 0x0300, 0);
	OAM_SetAttributes(3, 0x2738, 0xC673, 0x0380, 0);

	OAM_RotateSprite(0, 0, 136, 204);
	OAM_RotateSprite(1, 0, 136, 204);
	OAM_RotateSprite(2, 0, 136, 204);
	OAM_RotateSprite(3, 0, 136, 204);

  OAM_SetBlendMode(ALPHA_BLEND, BLEND(16, 16));

	OAM_CopySprites();
}