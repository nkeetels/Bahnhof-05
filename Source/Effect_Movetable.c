#include "KDEngine.h"
#include "KDPostProcessing.h"
#include "Effect_Movetable.h"
#include "gfx_solar.h"

extern int scanlineCounter;

extern unsigned char metal_image[16384];
extern unsigned char metal_palette[512];
extern const unsigned short tunnellut3[];
extern signed short lut_kusma[]; 

extern const unsigned short lut_metro[];
extern const unsigned short lut_sphere[];

extern const unsigned short metalBitmap[8192];
extern const unsigned short metalPal[256];

extern const unsigned short bahnhof1Tiles[2048];
extern const unsigned short bahnhof2Tiles[2048];
extern const unsigned short bahnhof3Tiles[2048];
extern const unsigned short bahnhof4Tiles[2048];
extern const unsigned short bahnhof1Pal[256];

extern const unsigned short solar_lTiles[2048];
extern const unsigned short solar_rTiles[2048];
extern const unsigned short solar_lPal[2048];

IWRAM_CODE void MoveTable(u8 *texture, s16 *table, int xoffset, int yoffset, int t)
{
	u32 *target = (u32*)(g_currentBuffer + 16*120);
	s16 *uv = (yoffset * 320) + xoffset + (table - 1);
	int i = 0, j = 0;

	#define PLOT \
	{ \
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
		*target++=(texture[((*(++uv))+t)&TEXSIZE]) | (texture[((*(++uv))+t)&TEXSIZE]<<8) | (texture[((*(++uv))+t)&TEXSIZE]<<16 | (texture[((*(++uv))+t)&TEXSIZE]<<24)); \     
	}
	const int endline = 160-19;
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
//	OAM_SetSprite(0, 0, 32, 64, 1, 0, gfx_solarTiles, gfx_solarPal);

	OAM_SetSprite(1, 120-32, 80-32, 64, 0, 0, solar_lTiles, solar_lPal);
	OAM_SetSprite(2, 120+32, 80-32, 64, 0, 0, solar_rTiles, solar_lPal);

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
    b = min(31, (b * b * v) >> 3)<<2;

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

  int x = (SinLUT[((time >> 1) + 64) & LUTSIZE] >> 1);
  int y = (SinLUT[(time) & LUTSIZE] >> 1);

	time &= 255;

	time <<= 1;
	int t = ((time << 8) | time) & 0x7fff;

  MoveTable(metalBitmap, lut_sphere, 40, 80, -t>>1);
}


void MovetableEffect_VCount()
{
    //u16 scanline = REG_VCOUNT << 4;
    //REG_BG2PD = 255-124 + ((SinLUT[((-scanlineCounter >> 9) + ((160 * 2 - scanline) >> 4)) & 255]) >> 1);

		u16 scanline = min(REG_VCOUNT, 159);

		REG_BLDCNT |= 0x00FF | ((0x0010) & 63);
    REG_BLDY = 16 - (60 + ((scanline - 80)) >> 3);
}
