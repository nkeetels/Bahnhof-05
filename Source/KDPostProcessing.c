// Bahnhof-05 Gameboy Advance demo source code
// 
// File: KDPostProcessing.c
// Description: various post-processing effects
//
//							Bloom: 		blur highlights based on 32-color base palette framebuffer
//										 		Usage: call in Update, should call bloom_Reset in both init and destroy
//
//							Contrast: color grading effect to palette contrast
//										    Usage: call in init or update, accepts any palette
//
//							Vignette: uses GBA fading registers to darken top and buttom 32 lines
//											  Update: call in VCount
//
//							Glitch: 	uses GBA background scaling registers to simulate scanline distortion
//											  Update: call in VCount
// 
// Note: the bloom effect uses additive feedback, hence the palette should be sorted according to intensity. 

#include "KDEngine.h"
#include "Zoomblur.h"

#define SOFTBLOOM 

extern u8 shadingtable[20][15];

static uint glitchCount = 0;

void Bloom_Reset()
{
  int i, j;
  for (i = 0; i < 15; i++)
  {
    shadingtable[0][i] = 0;
    shadingtable[19][i] = 0;
  }

  for (j = 0; j < 20; j++)
  {
    shadingtable[j][0] = 0;
    shadingtable[j][14] = 0;
  }
}

void IWRAM_CODE Post_Bloom(const u8 bloom_bias, const u8 bloom_threshold)
{
  int i, j, u, x, y;
  int offset = 0;

  for (i = 0; i < 15; i++)
  {
    shadingtable[0][i] = 0;
    shadingtable[19][i] = 0;
  }

  for (j = 0; j < 20; j++)
  {
    shadingtable[j][0] = 0;
    shadingtable[j][14] = 0;
  }

  u16 *target = g_currentBuffer;
  u8 *texels = (u8*)BACKBUFFER;
  
	for (y = 1; y < 19; y++)
	for (x = 1; x < 14; x++)  
	{
		u8 paletteIndex = texels[(y << 4) * 120 + (x << 4)];

		u16 color = ((unsigned short*)0x5000000)[paletteIndex];

		u8 r = (color >> 10) & 31;
		u8 g = (color >> 5) & 31;
		u8 b = color & 31;

		color = (r + g + b) + bloom_bias;

		shadingtable[y][x] = color > bloom_threshold ? 128 : 0;
	}

#ifdef SOFTBLOOM
	for (y = 1; y < 19; y++)
	for (x = 1; x < 14; x++)  
	{
		shadingtable[y][x] = _div((shadingtable[y][x] + shadingtable[y-1][x] + shadingtable[y+1][x] + shadingtable[y][x-1] + shadingtable[y][x+1]), 8);
	}

	const int DARKEN = 0;    
#else
  const int DARKEN = 1;
#endif

	int color = 0;
	int u1, u2, u3, u4;
	int s1,s2;
	const int tab = 15;
	offset = 0, x = 0, y = 0;
	int tablepos = 0;
	int ymul = 0;
	int du1, du2, eu1;
	int tempu1, tempu2;

	for (j = 0; j < 20-1; j++)
	{
		for (i = 0; i < 15-1; i++)
		{  
			u1 = shadingtable[j][i];
			u2 = shadingtable[j][i+1];
			u3 = shadingtable[j+1][i];
			u4 = shadingtable[j+1][i+1];
			
			du1 = (((u2 - u1) << 8) >> 3);
			du2 = (((u4 - u3) << 8) >> 3);

			ymul = (j << 3) * 120 + (i << 3);

			for (y = 0; y < 8; y += 2)
			{
				s1 = u1;
				s2 = u3;
				tempu1 = tempu2 = 0;
				{                           
					#define PLOT(add) \
					{ \
						tempu1 += du1; tempu2 += du2; s1 = u1 + (tempu1 >> 8); s2 = u3 + (tempu2 >> 8); \
						eu1 = (((s2 - s1) << 8) >> 3); \
						u = (s1 + ((eu1 * y) >> 8)); \
						u >>= DARKEN; \
						offset = ymul + add; \
						color = ((u << 8) | u); target[offset] += color; target[offset + 120] += color; \
					}

					PLOT(0)
					PLOT(1)
					PLOT(2)
					PLOT(3)
					PLOT(4)
					PLOT(5)
					PLOT(6)
					PLOT(7)
				}
				ymul += 240;
			} 
		}
		tablepos += tab;
	}	   
}


void Post_Contrast(u16 *dest, const u16 *palette, u8 v)
{
  for (int i = 0; i < 255; i++)
  {
    u16 color = palette[i];

    u16 r = (color >> 10) & 31;
    u16 g = (color >> 5) & 31;
    u16 b = color & 31;    

    r = min(31, r * v);
    g = min(31, g * v);
    b = min(31, b * v);

    dest[i] = (r << 10) | (g << 5) | b;
  }
}

void VCount_Vignette()
{
		u16 scanline = min(REG_VCOUNT, 159);

		int ramp = 0;
		if (scanline < 32)
		{
			ramp = 16 - (scanline >> 1);
		}
		else if (scanline > 160-32)
		{
			ramp = 16 - ((160 - scanline) >> 1);
		}
			
		REG_BLDCNT |= 0x00FF | ((0x0010) & 63);
		REG_BLDY = ramp;
}

void VCount_Glitch()
{
  u16 scanline = REG_VCOUNT;

  int amp = SinLUT[(glitchCount >> 9)&255] >> 3;
  int phase = _div(SinLUT[((scanline << 5) + (glitchCount >> 6)) & 255], 320 - amp * amp);

  REG_BG2PA = 256 - phase * phase;

	glitchCount++;
}