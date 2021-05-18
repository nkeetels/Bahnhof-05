// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: Effect_Movetale.c
// Description: the movetable effect is essentially a texture-coordinate rotation effect. Inputs are a 128 x 128 
//              8-bit indexed color texture, a pre-generated 320 x 256 (tunnel) lookup table, offsets for scrolling 
//              through the lookup table and a (time) index that gets added to the texture-coordinates in order to 
//              create the appearance of a moving camera.

#include "KDEngine.h"
#include "KDPostProcessing.h"
#include "Effect_Movetable.h"
#include "gfx_solar.h"

extern int scanlineCounter;

extern unsigned char metal_image[16384];
extern unsigned char metal_palette[512];
extern const unsigned short tunnellut3[];
extern signed short lut_kusma[]; 

extern const unsigned short metalBitmap[8192];
extern const unsigned short metalPal[256];

extern const unsigned short bahnhof1Tiles[2048];
extern const unsigned short bahnhof2Tiles[2048];
extern const unsigned short bahnhof3Tiles[2048];
extern const unsigned short bahnhof4Tiles[2048];
extern const unsigned short bahnhof1Pal[256];

IWRAM_CODE void MoveTable(u8 *texture, s16 *table, int xoffset, int yoffset, int t)
{
	u32 *target = (u32*)(g_currentBuffer);
	s16 *uv = (yoffset * 320) + xoffset + (table - 1);
	int i = 0, j = 0;	

	#define PLOT \
	{ \
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
	}
	const int endline = 160 - 40;
	const int line = 320 - 240;
  do
	{
		i = 0;
    do
		{
			PLOT
			PLOT
			PLOT
		} while (++i < 5);
		uv += line;
    } while (++j < endline);
}

void MovetableEffect_Init()
{
  REG_DISPCNT = 0x1444;


	REG_BLDCNT = 0x00FF; 
	OAM_Clear();
	OAM_SetSprite(0, 120-64, 60-32, 64, 1, 0, gfx_solarTiles, gfx_solarPal);
//	OAM_SetSprite(1, 120-60, 60-28, 64, 1, 0, gfx_solarTiles, gfx_solarPal);
  OAM_SetBlendMode(ALPHA_BLEND, BLEND(16,16));
  OAM_CopySprites();


  int i;
	int v = 9;
  for (i = 0; i < 256; i++)
  {
    u8 r = (metalPal[i] & 0x7C00) >> 10;
    u8 g = (metalPal[i] & 0x3E0) >> 5;
    u8 b = (metalPal[i] & 0x1F);

    r = min(31, (r * r * v) >> 3);
    g = min(31, (g * g * v) >> 3);
    b = min(31, (b * b * v) >> 3);

    ((unsigned short*)0x5000000)[i] = (r << 10) | (r << 5) | g;
  }  

  const u8 palEntry = 139;
  const u16 bgColor = (palEntry << 8) | palEntry; 
  FastSet16(g_currentBuffer, bgColor, (SCREENWIDTH*SCREENHEIGHT)>>1);  

	//OAM_Overlay(bahnhof1Pal, bahnhof1Tiles, bahnhof2Tiles, bahnhof3Tiles, bahnhof4Tiles);
}

void MovetableEffect_Destroy()
{
		OAM_Clear();
		OAM_CopySprites();

		REG_BLDY = 16;

	REG_BG2PA = 256;
  REG_BG2PB = 0;
  REG_BG2PC = 0;
  REG_BG2PD = 256;
}

extern const s16 SinLUT[];
#define LUTSIZE 255
void MovetableEffect_Update(uint time)
{
	time += 50;

	OAM_RotateSprite(0, time, 200, 200);
	OAM_CopySprites();

  int x = 40 + (SinLUT[((time >> 1) + 64) & LUTSIZE] >> 3);
  int y = 60 + (SinLUT[(time) & LUTSIZE] >> 3);

	int t = (time<<1) & LUTSIZE;

  MoveTable(metalBitmap, lut_kusma, x, y, -t);

}


void MovetableEffect_VCount()
{
    u16 scanline = REG_VCOUNT << 4;
    REG_BG2PD = 255-124 + ((SinLUT[((-scanlineCounter >> 9) + ((160 * 2 - scanline) >> 4)) & 255]) >> 1);

		VCount_Vignette();
}
