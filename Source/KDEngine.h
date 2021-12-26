// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDEngine.h
// Description: KDEngine was originally intended to cover rendering and polygon rasterization in GBA Mode 4.

#ifndef __KDENGINE_H
#define __KDENGINE_H

#include "KDMemory.h"
#include "KDRegs.h"
#include "KDMath.h"
#include "KDOam.h"
#include "KDVideo.h"
#include "KDClipping.h"

#define TEXSIZE 0x3fff

typedef struct Vertex
{
  Vector3 m_position;
  u8 u, v;
  u8 s, t;
} KDVertex;

typedef struct Face
{
  struct Vertex m_vertices[3];
  struct Face *m_next;
  //u16 m_depth;
  s8 m_normal;  
} KDFace;

#define MODEL_TEXTURED 1 << 1
#define MODEL_SORTFACES 1 << 2
#define MODEL_BACKFACECULLING 1 << 3
#define MODEL_ENVMAPPED 1 << 4
#define MODEL_WIREFRAME 1 << 5

typedef struct Model
{
  struct Face *m_faces;
  Vector3 m_position;
  Vector3 m_rotation;
  s8 m_cullBack;
  s8 m_flags;
  u16 m_blend;
} KDModel;

extern u16 *g_currentBuffer;
extern KDModel currentModel;
extern KDFace currentFaces[];

void Render(KDModel *model);
void LoadModel(KDModel *model, KDFace f[], const s16 positions[], const s16 normals[], const u16 texcoords[], int numVertices, int scale, int textured);
void LoadModelFlat(KDModel *model, KDFace f[], const s16 positions[], int numVertices, int scale);
void make_cube(KDModel *model, KDFace f[], u16 size, int textured);
void SetTexture(u8 *texture, u8 palette_offset);
void SetBumpMap(u8 *bumpmap);

void HLine(int x1, int x2, int y, u16 color);
void AALine(int x0, int y0, int x1, int y1, u8 color);
void flatTriangle(KDFace *face, u16 color);
void wireTriangle(KDFace *face, u16 color);

void TransformLocalToWorldProjection(KDModel *model, KDVertex *v, Matrix3x3 m);
void TransformToScreenSpace(KDVertex *v);

#endif//__KDENGINE_H
