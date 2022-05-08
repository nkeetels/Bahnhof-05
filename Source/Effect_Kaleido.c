// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: OBJ-test.c
// Description: this effect simply renders a textured-bumpmapped cube. 
//
// Note: the displayed sprites (picture + text) are actually one large 64 x 192 tiled image with 224 colors reserved 
//       for the picture and 32 colors reserved for the text.

#include "KDEngine.h"
#include "KDPostProcessing.h"
#include "Effect_Kaleido.h"

#define KALEIDO_SIDES 10

extern const unsigned short metalBitmap[8192];
extern const unsigned short metalPal[256];
extern const unsigned short tex_godwatbotBitmap[8192];
extern const unsigned short tex_godwatbotPal[256];


extern const unsigned short atgtexBitmap[8192];
extern const unsigned short atgtexPal[256] ;


void RotatePalette(uint time)
{ 
  int i;
  for (i = 0; i < 256; i++)
  {
    u8 r = (tex_godwatbotPal[i] & 0x7C00) >> 10;
    u8 g = (tex_godwatbotPal[i] & 0x3E0) >> 5;
    u8 b = (tex_godwatbotPal[i] & 0x1F);

    r += (SinLUT[time>>1] >> 2);
    g += (SinLUT[time>>1] >> 2);
    b += (SinLUT[time>>1] >> 2);

    ((unsigned short*)0x5000000)[i] = (b << 10) | (g << 5) | r;
  }

}

void Kaleido_Init()
{
  REG_DISPCNT = 0x1444;
  FastCopy16(((unsigned short*)0x5000000), tex_godwatbotPal, 256);

  UpdateKaleidoscope(&currentModel, currentFaces, KALEIDO_SIDES, 0);  

  SetTexture(tex_godwatbotBitmap, 0);
  
  currentModel.m_faces = &currentFaces[0];
  for (int i = 0; i < KALEIDO_SIDES; i ++)
  {
    currentFaces[i].m_next = &currentFaces[i+1];
  }
  currentModel.m_faces[KALEIDO_SIDES-1].m_next = 0;

  currentModel.m_rotation.x = 0;
  currentModel.m_rotation.y = 0;
  currentModel.m_rotation.z = 0;
  currentModel.m_position.x = 0;
  currentModel.m_position.y = 0;
  currentModel.m_position.z = 0;
  currentModel.m_flags =/* MODEL_BACKFACECULLING | MODEL_SORTFACES |*/ MODEL_TEXTURED;

  RotatePalette(255);
}

void Kaleido_Destroy()
{
}

void UpdateKaleidoscope(KDModel *model, KDFace f[], int sides, int time)
{
  int radius = 64;
  int x, y;
  float step;

  const int angle = (time);//64 + ((SinLUT[time & LUTSIZE]) >> 2);
  const int zoom = 16;// + (SinLUT[time*3 & LUTSIZE] >> 5);

  Vector2 center;

  center.x = 32 + ((SinLUT[(64 + (time << 2))&LUTSIZE]) >> 1);
  center.y = 64 + ((SinLUT[(00 + (time << 1) - 85)&LUTSIZE]) >> 1);

  Vector2 tc0, tc1, tc2;

  tc0.x = center.x + ((zoom * SinLUT[(64 + angle - 85)&LUTSIZE]) >> 6);
  tc0.y = center.y + ((zoom * SinLUT[(00 + angle - 85)&LUTSIZE]) >> 6);
  tc1.x = center.x + ((zoom * SinLUT[(64 + angle)&LUTSIZE]) >> 6);
  tc1.y = center.y + ((zoom * SinLUT[(00 + angle)&LUTSIZE]) >> 6);
  tc2.x = center.x + ((zoom * SinLUT[(64 + angle + 85)&LUTSIZE]) >> 6);
  tc2.y = center.y + ((zoom * SinLUT[(00 + angle + 85)&LUTSIZE]) >> 6);

  for (int i = 0; i < sides; i++)
  {
    x = 0;
    y = 0;

    f[i].m_vertices[0].m_position.x = x;
    f[i].m_vertices[0].m_position.y = y;
    f[i].m_vertices[0].m_position.z = 0;

    step = (float)sides / i;
    f[i].m_vertices[1].m_position.x = SinLUT[(int)(64 + (LUTSIZE+1) / step) & LUTSIZE] >> 2;
    f[i].m_vertices[1].m_position.y = (2 * SinLUT[(int)((LUTSIZE+1) / step) & LUTSIZE]) >> 3;
    f[i].m_vertices[1].m_position.z = 0;

			
    step = (float)sides / (i + 1);
    f[i].m_vertices[2].m_position.x = SinLUT[(int)(64 + (LUTSIZE+1) / step) & LUTSIZE] >> 2;
    f[i].m_vertices[2].m_position.y = (2 * SinLUT[(int)((LUTSIZE+1) / step) & LUTSIZE]) >> 3;
    f[i].m_vertices[2].m_position.z = 0;

    f[i].m_vertices[0].u = tc0.x;
    f[i].m_vertices[0].v = tc0.y;
    f[i].m_vertices[1].u = ((i & 1) == 0) ? tc1.x : tc2.x;
    f[i].m_vertices[1].v = ((i & 1) == 0) ? tc1.y : tc2.y;
    f[i].m_vertices[2].u = ((i & 1) == 0) ? tc2.x : tc1.x;
    f[i].m_vertices[2].v = ((i & 1) == 0) ? tc2.y : tc1.y;
  }
}

void Kaleido_Update(uint time)
{
  FastSet16(g_currentBuffer, (255<<8)|(255), (SCREENWIDTH*SCREENHEIGHT)>>1);


  UpdateKaleidoscope(&currentModel, currentFaces, KALEIDO_SIDES, time>>2);  

  //currentModel.m_rotation.x = SinLUT[(time>>1) & LUTSIZE] >> 2;
  currentModel.m_position.z = 130;

  Render(&currentModel);   
}

void Kaleido_VCount(uint time)
{

}
