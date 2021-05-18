// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: OBJ-test.c
// Description: this effect simply renders a textured-bumpmapped cube. 
//
// Note: the displayed sprites (picture + text) are actually one large 64 x 192 tiled image with 224 colors reserved 
//       for the picture and 32 colors reserved for the text.

#include "KDEngine.h"
#include "KDPostProcessing.h"
#include "Effect_Greetings.h"

extern const unsigned short greetsBitmap[8192];
extern const unsigned short greetsPal[256];

extern const unsigned short frakkelBitmap[8192];
extern const unsigned short frakkelPal[256];

extern const unsigned short greetings1Tiles[2048];
extern const unsigned short greetings2Tiles[2048];
extern const unsigned short greetings3Tiles[2048];
extern const unsigned short greetings4Tiles[2048];
extern const unsigned short greetings1Pal[256];

extern int OBJ_faceIdx;

void CreatePlane(int x1, int y1, int z1, u8 u1, u8 v1, int x2, int y2, int z2, u8 u2, u8 v2, int x3, int y3, int z3, u8 u3, u8 v3, int x4, int y4, int z4, u8 u4, u8 v4)
{
    currentFaces[OBJ_faceIdx + 0].m_vertices[0].m_position.x = x1;
    currentFaces[OBJ_faceIdx + 0].m_vertices[0].m_position.y = y1;
    currentFaces[OBJ_faceIdx + 0].m_vertices[0].m_position.z = z1;
    currentFaces[OBJ_faceIdx + 0].m_vertices[0].u = u1;
    currentFaces[OBJ_faceIdx + 0].m_vertices[0].v = v1 >> 1;
    currentFaces[OBJ_faceIdx + 0].m_vertices[1].m_position.x = x2;
    currentFaces[OBJ_faceIdx + 0].m_vertices[1].m_position.y = y2;
    currentFaces[OBJ_faceIdx + 0].m_vertices[1].m_position.z = z2;
    currentFaces[OBJ_faceIdx + 0].m_vertices[1].u = u2;
    currentFaces[OBJ_faceIdx + 0].m_vertices[1].v = v2 >> 1;
    currentFaces[OBJ_faceIdx + 0].m_vertices[2].m_position.x = x4;
    currentFaces[OBJ_faceIdx + 0].m_vertices[2].m_position.y = y4;
    currentFaces[OBJ_faceIdx + 0].m_vertices[2].m_position.z = z4;    
    currentFaces[OBJ_faceIdx + 0].m_vertices[2].u = u4;
    currentFaces[OBJ_faceIdx + 0].m_vertices[2].v = v4 >> 1;

    currentFaces[OBJ_faceIdx + 1].m_vertices[2].m_position.x = x2;
    currentFaces[OBJ_faceIdx + 1].m_vertices[2].m_position.y = y2;
    currentFaces[OBJ_faceIdx + 1].m_vertices[2].m_position.z = z2;
    currentFaces[OBJ_faceIdx + 1].m_vertices[2].u = u2;
    currentFaces[OBJ_faceIdx + 1].m_vertices[2].v = v2 >> 1;
    currentFaces[OBJ_faceIdx + 1].m_vertices[1].m_position.x = x4;
    currentFaces[OBJ_faceIdx + 1].m_vertices[1].m_position.y = y4;
    currentFaces[OBJ_faceIdx + 1].m_vertices[1].m_position.z = z4;
    currentFaces[OBJ_faceIdx + 1].m_vertices[1].u = u4;
    currentFaces[OBJ_faceIdx + 1].m_vertices[1].v = v4 >> 1;
    currentFaces[OBJ_faceIdx + 1].m_vertices[0].m_position.x = x3;
    currentFaces[OBJ_faceIdx + 1].m_vertices[0].m_position.y = y3;
    currentFaces[OBJ_faceIdx + 1].m_vertices[0].m_position.z = z3;   
    currentFaces[OBJ_faceIdx + 1].m_vertices[0].u = u3;
    currentFaces[OBJ_faceIdx + 1].m_vertices[0].v = v3 >> 1;   

    OBJ_faceIdx += 2;
}

const int plane_halfWidth = 128>>1;
const int plane_halfHeight = 30>>1;
const int plane_halfCeiling = 50>>1;
const int plane_halfPillar = 10>>1;

void GenerateSceditsScene()
{
  const int v_h = 128 >> 2;

  int v_ofs = v_h;
  CreatePlane(-plane_halfWidth, -plane_halfHeight, -50, 0, 0, plane_halfWidth, -plane_halfHeight, -50, 120, 0, plane_halfWidth, plane_halfHeight, -50, 120, v_ofs-6, -plane_halfWidth, plane_halfHeight, -50, 0, v_ofs-6);
  
  int offs = -40;
  CreatePlane(-plane_halfWidth, -plane_halfHeight + offs, -50, 0, v_ofs, plane_halfWidth, -plane_halfHeight + offs, -50, 120, v_ofs, plane_halfWidth, plane_halfHeight + offs, -50, 120, v_ofs * 2-6, -plane_halfWidth, plane_halfHeight + offs, -50, 0, v_ofs*2-6);

  v_ofs += v_h;
  offs = 40;
  CreatePlane(-plane_halfWidth, -plane_halfHeight + offs, -50, 0, v_ofs, plane_halfWidth, -plane_halfHeight + offs, -50, 120, v_ofs, plane_halfWidth, plane_halfHeight + offs, -50, 120, v_ofs + v_h-6, -plane_halfWidth, plane_halfHeight + offs, -50, 0, v_ofs + v_h-8);
}

void Greetings_Init()
{
  OBJ_faceIdx = 0;
  int i;
  REG_DISPCNT = 0x1444;
  FastCopy16(((unsigned short*)0x5000000),frakkelPal, 256); 

  OBJ_faceIdx = 0;
  GenerateSceditsScene();
 
  currentModel.m_faces = &currentFaces[0];
  for (i = 0; i < OBJ_faceIdx - 1; i ++)
  {
    currentFaces[i].m_next = &currentFaces[i+1];
  }
  currentModel.m_faces[OBJ_faceIdx - 1].m_next = 0;

  SetTexture((u8*)greetsBitmap, 0);

  currentModel.m_rotation.x = 0;
  currentModel.m_rotation.y = 0;
  currentModel.m_rotation.z = 0;
  currentModel.m_position.x = 0;
  currentModel.m_position.y = 0;
  currentModel.m_position.z = 0;

  currentModel.m_flags = MODEL_BACKFACECULLING | MODEL_SORTFACES | MODEL_TEXTURED;

  OAM_Overlay(greetings1Pal, greetings1Tiles, greetings2Tiles, greetings3Tiles, greetings4Tiles);
}

void Greetings_Destroy()
{
}

void RotateAndZoom(uint time)
{
  s16 angle = 0;

  s16 tempC = SinLUT[((angle>>7) + 64)&255]>>1;
  s16 tempS = SinLUT[((angle>>7)&255)]>>1;
  s16 zoom = (-256 + 512 * 2 + SinLUT[((time >> 2) - 32) & 255]) >> 1;
	s16 dx = (tempC * zoom) >> 8;
	s16 dy = (tempS * zoom) >> 8;
	s16 hx = (-tempS * zoom) >> 8;
	s16 hy = (tempC * zoom) >> 8;

  REG_BG2PA = dx;
  REG_BG2PB = dy;
  REG_BG2PC = hx;
  REG_BG2PD = hy;
}

void Greetings_Update(uint time)
{
  FastCopy16(g_currentBuffer, frakkelBitmap, 160*120);
 
  currentModel.m_rotation.y = (-45 + SinLUT[(time >> 1) & 255]) >> 4;
  currentModel.m_rotation.z = SinLUT[(time) & 255] >> 3;

  Render(&currentModel);   

  u8 m = 3 - ((time >> 3) & 3);
  Post_Contrast((unsigned short*)0x5000000, frakkelPal, m);
  
  RotateAndZoom(time);
}
