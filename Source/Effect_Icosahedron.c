#include "KDEngine.h"
#include "KDPostProcessing.h"
#include "Effect_Icosahedron.h"

extern const unsigned short master_Palette[256];
extern const unsigned char press_128x64_1_Bitmap[];
extern const unsigned char press_128x64_2_Bitmap[];
extern const unsigned short tex_litsphereBitmap[8192];
extern const unsigned short tex_litspherePal[256];


extern const unsigned short icosa1Tiles[2048];
extern const unsigned short icosa2Tiles[2048];
extern const unsigned short icosa3Tiles[2048];
extern const unsigned short icosa4Tiles[2048];
extern const unsigned short icosa1Pal[256];

const s16 X = 64 * .525731112119133606f;
const s16 Z = 64 * .850650808352039932f;
const s16 N = 0.f;

const Vector3 ico_verts[] = 
{
  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z}, {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X}, {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
};

const u8 ico_indices[] = 
{
  0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
};

void Icosahedron_CreateIndexBuffer(const int numFaces)
{
  currentModel.m_faces = &currentFaces[0];
  for (int i = 0; i < numFaces - 1; i++)
  {
    currentFaces[i].m_next = &currentFaces[i+1];
  }
    
  currentModel.m_faces[numFaces - 1].m_next = 0;
}

void Icosahedron_Generate(int time)
{
  int index = 0;
  int i = 0;

  for (i = 0; i < 20; i++)
  {
    currentFaces[i].m_vertices[0].m_position.x = ico_verts[ico_indices[index + 0]].x;
    currentFaces[i].m_vertices[0].m_position.y = ico_verts[ico_indices[index + 0]].y;
    currentFaces[i].m_vertices[0].m_position.z = ico_verts[ico_indices[index + 0]].z;
    currentFaces[i].m_vertices[1].m_position.x = ico_verts[ico_indices[index + 1]].x;
    currentFaces[i].m_vertices[1].m_position.y = ico_verts[ico_indices[index + 1]].y;
    currentFaces[i].m_vertices[1].m_position.z = ico_verts[ico_indices[index + 1]].z;
    currentFaces[i].m_vertices[2].m_position.x = ico_verts[ico_indices[index + 2]].x;
    currentFaces[i].m_vertices[2].m_position.y = ico_verts[ico_indices[index + 2]].y;
    currentFaces[i].m_vertices[2].m_position.z = ico_verts[ico_indices[index + 2]].z;

    index += 3;
  }

  Icosahedron_CreateIndexBuffer(20);
}

void Icosahedron_Subdivide(int time)
{
  int i = 0, j = 0, index = 0;
  Vector3 edge1, edge2, edge3;

  for (i = 0; i < 20; i++)
  {
    s16 t0x0 = ico_verts[ico_indices[index + 0]].x;
    s16 t0y0 = ico_verts[ico_indices[index + 0]].y;
    s16 t0z0 = ico_verts[ico_indices[index + 0]].z;

    s16 t0x1 = ico_verts[ico_indices[index + 1]].x;
    s16 t0y1 = ico_verts[ico_indices[index + 1]].y;
    s16 t0z1 = ico_verts[ico_indices[index + 1]].z;

    s16 t0x2 = ico_verts[ico_indices[index + 2]].x;
    s16 t0y2 = ico_verts[ico_indices[index + 2]].y;
    s16 t0z2 = ico_verts[ico_indices[index + 2]].z;
  
    edge1.x = t0x0 + ((t0x1 - t0x0) >> 1);
    edge1.y = t0y0 + ((t0y1 - t0y0) >> 1);
    edge1.z = t0z0 + ((t0z1 - t0z0) >> 1);

    edge2.x = t0x1 + ((t0x2 - t0x1) >> 1);
    edge2.y = t0y1 + ((t0y2 - t0y1) >> 1);
    edge2.z = t0z1 + ((t0z2 - t0z1) >> 1);

    edge3.x = t0x2 + ((t0x0 - t0x2) >> 1);
    edge3.y = t0y2 + ((t0y0 - t0y2) >> 1);
    edge3.z = t0z2 + ((t0z0 - t0z2) >> 1);

    currentFaces[j].m_vertices[0].m_position.x = edge3.x;
    currentFaces[j].m_vertices[0].m_position.y = edge3.y;
    currentFaces[j].m_vertices[0].m_position.z = edge3.z;
    currentFaces[j].m_vertices[1].m_position.x = t0x0;
    currentFaces[j].m_vertices[1].m_position.y = t0y0;
    currentFaces[j].m_vertices[1].m_position.z = t0z0;
    currentFaces[j].m_vertices[2].m_position.x = edge1.x;
    currentFaces[j].m_vertices[2].m_position.y = edge1.y;
    currentFaces[j].m_vertices[2].m_position.z = edge1.z;
    j++;

    currentFaces[j].m_vertices[0].m_position.x = edge1.x;
    currentFaces[j].m_vertices[0].m_position.y = edge1.y;
    currentFaces[j].m_vertices[0].m_position.z = edge1.z;
    currentFaces[j].m_vertices[1].m_position.x = t0x1;
    currentFaces[j].m_vertices[1].m_position.y = t0y1;
    currentFaces[j].m_vertices[1].m_position.z = t0z1;
    currentFaces[j].m_vertices[2].m_position.x = edge2.x;
    currentFaces[j].m_vertices[2].m_position.y = edge2.y;
    currentFaces[j].m_vertices[2].m_position.z = edge2.z;
    j++;

    currentFaces[j].m_vertices[0].m_position.x = edge2.x;
    currentFaces[j].m_vertices[0].m_position.y = edge2.y;
    currentFaces[j].m_vertices[0].m_position.z = edge2.z;
    currentFaces[j].m_vertices[1].m_position.x = t0x2;
    currentFaces[j].m_vertices[1].m_position.y = t0y2;
    currentFaces[j].m_vertices[1].m_position.z = t0z2;
    currentFaces[j].m_vertices[2].m_position.x = edge3.x;
    currentFaces[j].m_vertices[2].m_position.y = edge3.y;
    currentFaces[j].m_vertices[2].m_position.z = edge3.z;
    j++;

    currentFaces[j].m_vertices[0].m_position.x = edge1.x;
    currentFaces[j].m_vertices[0].m_position.y = edge1.y;
    currentFaces[j].m_vertices[0].m_position.z = edge1.z;
    currentFaces[j].m_vertices[1].m_position.x = edge2.x;
    currentFaces[j].m_vertices[1].m_position.y = edge2.y;
    currentFaces[j].m_vertices[1].m_position.z = edge2.z;
    currentFaces[j].m_vertices[2].m_position.x = edge3.x;
    currentFaces[j].m_vertices[2].m_position.y = edge3.y;
    currentFaces[j].m_vertices[2].m_position.z = edge3.z;
    j++;
    
    index += 3;
  }

  Icosahedron_CreateIndexBuffer(79);
}

void GeneratePalette()
{
  for (int i = 0; i < 256; i++)
  { 
    u8 r0 = i * i * i >> 16;
    u8 g0 = i * i * i >> 16;
    u8 b0 = i;
    u8 r1 = i;
    u8 g1 = i;
    u8 b1 = i;

    int t = 128;

    u8 r = lerp(r0, r1, t);
    u8 g = lerp(g0, g1, t);
    u8 b = lerp(b0, b1, t);
    
    int color = RGB15(r ,g, b);

    ((unsigned short*)0x5000000)[i] = color;
  }
}

void Icosahedron_Init()
{
  int i = 0;

  REG_DISPCNT = 0x1444;
  FastCopy16(((unsigned short*)0x5000000),tex_litspherePal, 256); 

  OAM_Overlay(icosa1Pal, icosa1Tiles, icosa2Tiles, icosa3Tiles, icosa4Tiles);

  for (i = 0; i < 256; i++)
  { 
    int color = RGB15(i * i * i >> 16, i * i * i >> 16, i);

    ((unsigned short*)0x5000000)[i] = color;
  }

  Icosahedron_Subdivide(0);

  currentModel.m_rotation.x = 0;
  currentModel.m_rotation.y = 0;
  currentModel.m_rotation.z = 0;
  currentModel.m_position.x = 0;
  currentModel.m_position.y = 0;
  currentModel.m_position.z = 0;
  currentModel.m_blend = 3;
  currentModel.m_flags = MODEL_BACKFACECULLING | MODEL_SORTFACES;// | MODEL_ENVMAPPED;  

  SetTexture((u8*)tex_litsphereBitmap, 0);
}

void Icosahedron_Destroy()
{
  OAM_Clear();
  OAM_CopySprites();  

  REG_BG2PA = 256;
  REG_BG2PB = 0;
  REG_BG2PC = 0;
  REG_BG2PD = 256;
}

void Icosahedron_Update(uint time)
{
  GeneratePalette();

  FastSet16(g_currentBuffer, (1<<8)|(1), (SCREENWIDTH*SCREENHEIGHT)>>1); 

  currentModel.m_position.y = 0;
  currentModel.m_rotation.x = -time>>1;
  currentModel.m_rotation.y = -time;
  currentModel.m_rotation.z = -time>>1;

  Render(&currentModel);     

  u8 tt = 3 - ((-time >> 3) & 3);
  tt >>= 1;
  for (int i = 0; i < 255; i++)
  {
    u16 color = icosa1Pal[i];

    u16 r = (color >> 10) & 31;
    u16 g = (color >> 5) & 31;
    u16 b = color & 31;    

    g = min(31, g >> tt);
    b = min(31, b >> tt);

    ((unsigned short*)0x5000200)[i] = (r << 10) | (g << 5) | b;
  }

}

void Icosahedron_VCount()
{
  VCount_Glitch();
}