// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDEngine.c
// Description: KDEngine handles polygon rasterization for GBA Mode 4. The texture should be a 128 x 128 
//              indexed color 8-bit bitmap.
//
// Note: the shading is honestly a bit of a hack. I created a tool in Python that quantizes an indexed color 8-bit
//       bitmap image to 32 colors and create 7 shades of that 32-color palette. This is why g_paletteOffset exists. 

#include "KDTypes.h"
#include "KDEngine.h"

#define MAX_FACES 140

extern uint g_frame;

u8 *g_currentBumpMap = 0;
u8 *g_currentTexture = 0;
u8 g_paletteOffset = 0;
u16 *g_currentBuffer = FRONTBUFFER;

KDModel currentModel;
KDFace EWRAM_DATA currentFaces[MAX_FACES];
KDFace transformedFaces[MAX_FACES];
u16 drawOrder[MAX_FACES];
u16 depthBuffer[MAX_FACES];

s16 clipInput[6][2];
s16 clipOutput[6][2];

int debugValue = 0;


extern unsigned char phong_lut[16384];

void swap_vertex(KDVertex *a, KDVertex *b)
{
  KDVertex temp = *a;
  *a = *b;
  *b = temp;
}

void SetTexture(u8 *texture, u8 palette_offset)
{
  g_currentTexture = texture;
  g_paletteOffset = palette_offset;
}

void SetBumpMap(u8 *bumpmap)
{
    g_currentBumpMap = bumpmap;
}


void PutPixel(int x, int y, u8 color)
{
   u16 *target = (u16*)g_currentBuffer + ((y * 240 + x) >> 1);

   if (x & 1)
   {
      *target = (*target & 0xFF) | (color << 8);
   }
   else
   {
      *target = (*target & ~0xFF) | color;
   }
}

void IWRAM_CODE HLine(int x1, int x2, int y, u16 color)
{
  x1 = max(1, x1);
  x2 = min(237, x2);

  u8 *vscreen = (u8*)((u32)g_currentBuffer + x1 + y * SCREENWIDTH);

  int length = x2 - x1 + 2;
  int n = (length + 7) >> 3;

  switch (length & 7)
  {
    case 0: do {  *vscreen++ = color;   
    case 7:       *vscreen++ = color;
    case 6:       *vscreen++ = color;
    case 5:       *vscreen++ = color; 
    case 4:       *vscreen++ = color;   
    case 3:       *vscreen++ = color;
    case 2:       *vscreen++ = color;
    case 1:       *vscreen++ = color;  
    } while (--n > 0); 
  }
}

void IWRAM_CODE flatTriangle(KDFace *face, u16 color) 
{
  int x1 = face->m_vertices[0].m_position.x;
  int y1 = face->m_vertices[0].m_position.y;
  int x2 = face->m_vertices[1].m_position.x;
  int y2 = face->m_vertices[1].m_position.y;  
  int x3 = face->m_vertices[2].m_position.x;
  int y3 = face->m_vertices[2].m_position.y;  

  if (y3 < y2)  
  {
    _swap(x3, x2);
    _swap(y3, y2);
  }
  if (y2 < y1) 
  {
    _swap(x2, x1);
    _swap(y2, y1);
  }
  if (y3 < y2) 
  { 
    _swap(x3, x2);
    _swap(y3, y2);
  }

  if (y3 == y1)
    return;

  int deltaAC = _div((x3 - x1) << 16, y3 - y1);
  int deltaAB = _div((x2 - x1) << 16, y2 - y1);
  int deltaBC = _div((x3 - x2) << 16, y3 - y2);

  int startx = (x1 << 16);
  int endx = startx;

  int mid = (x1 << 16) + (y2 - y1) * deltaAC;

  int swapped = 0;
  if ((x2 << 16) < mid) 
  {
    _swap(deltaAC, deltaAB);
    swapped = 1;
  }

  int y = 0;
  if (y1 != y2) 
  {
    y1 = max(1, y1);
    y2 = min(160, y2);

    for (y = y1; y < y2; y ++) 
    {
      HLine((startx+=deltaAC) >> 16, (endx+=deltaAB) >> 16, y, color);
    }
  }
  else if (y1 == y2) 
  {
    if (!swapped) 
    {
      startx = (x1 << 16);
      endx = (x2 << 16);
    } 
    else 
    {
      startx = (x2 << 16);
      endx = (x1 << 16);
    }
  }

  if (swapped) 
  {
    _swap(deltaAB, deltaAC);
  }

  if ((x2 << 16) < mid) 
  {
    _swap(deltaAC, deltaBC);
  }

  if (y2 != y3) 
  {
    y2 = max(1, y2);
    y3 = min(159, y3);

    for (y = y2; y <= y3; y ++) 
    {
      HLine(startx >> 16, endx >> 16, y, color);
      startx += deltaAC;
      endx += deltaBC;
    }
  } 
  else 
  {
    y = y2;
    HLine(startx >> 16, endx >> 16, y, color);
  }  
}


#define SWAPPED 1 << 1

void IWRAM_CODE TextureTriangle(KDFace *face, u8 *texture, u8 *bumpmap, u8 shade)
{
  if (face->m_vertices[1].m_position.y < face->m_vertices[0].m_position.y)
   swap_vertex(&face->m_vertices[0], &face->m_vertices[1]);
  if (face->m_vertices[2].m_position.y < face->m_vertices[0].m_position.y)
   swap_vertex(&face->m_vertices[0], &face->m_vertices[2]);
  if (face->m_vertices[2].m_position.y < face->m_vertices[1].m_position.y)
   swap_vertex(&face->m_vertices[1], &face->m_vertices[2]);

  if (face->m_vertices[0].m_position.y == face->m_vertices[2].m_position.y || face->m_vertices[2].m_position.y <= 0 || face->m_vertices[0].m_position.y >= 160)
    return;

  int x0 = face->m_vertices[0].m_position.x << 16;
  int y0 = face->m_vertices[0].m_position.y;  
  int u0 = face->m_vertices[0].u << 8;
  int v0 = face->m_vertices[0].v << 8;

  int x1 = face->m_vertices[1].m_position.x << 16;
  int y1 = face->m_vertices[1].m_position.y;
  int u1 = face->m_vertices[1].u << 8;
  int v1 = face->m_vertices[1].v << 8;

  int x2 = face->m_vertices[2].m_position.x << 16;
  int y2 = face->m_vertices[2].m_position.y;
  int u2 = face->m_vertices[2].u << 8;
  int v2 = face->m_vertices[2].v << 8;

  if (y0 == y1)
  {
    if (x1 < x0)
    {
      _swap(x0, x1);
      _swap(u0, u1);
      _swap(v0, v1);
    }

    y1++; 
  }

  if (y1 == y2)
    y2++;

  y1 = min(y1, 160);
  y2 = min(y2, 160);

  int length, dxAC, duAC, dvAC, dxBC, duBC, dvBC, dxAB, duAB, dvAB;

  length = y2 - y0;
  dxAC = _div((x2 - x0), length);
  duAC = _div((u2 - u0), length);
  dvAC = _div((v2 - v0), length);

  length = y1 - y0;
  dxAB = _div((x1 - x0), length);
  duAB = _div((u1 - u0), length);
  dvAB = _div((v1 - v0), length);

  length = y2 - y1;
  dxBC = _div((x2 - x1), length);
  duBC = _div((u2 - u1), length);
  dvBC = _div((v2 - v1), length);

  u8 flags = 0;

  if (dxAB < dxAC)
  {
    flags |= SWAPPED;

    _swap(dxAB, dxAC);
    _swap(duAB, duAC);
    _swap(dvAB, dvAC);
  }

  x2 = x1;
  u2 = u1;
  v2 = v1;
  x1 = x0;
  u1 = u0;
  v1 = v0;    

  if (y0 < 0)
  {
    int overclip = y0;

    x0 -= overclip * dxAC;
    x1 -= overclip * dxAB;
    u0 -= overclip * duAC;
    u1 -= overclip * duAB;
    v0 -= overclip * dvAC;
    v1 -= overclip * dvAB;

    y0 = 0;
  }

  int dy, b0, b1, du, dv, ds, dt;

  dy = y1 - y0;
  b0 = x0 + (dy * dxAC);
  b1 = x1 + (dy * dxAB);
  length = ((b1 - b0) >> 16);

  b0 = u0 + (dy * duAC);
  b1 = u1 + (dy * duAB);
  du = _div(((b1 - b0) << 8), length) >> 8;

  b0 = v0 + (dy * dvAC);
  b1 = v1 + (dy * dvAB);
  dv = _div(((b1 - b0) << 8), length) >> 8;

  // to do: fix HS clipping
  x0 = max(0, x0);
  x1 = max(0, x1);

  u16 *line;
  
  int stride, n, y, tu, tv, ts, tt;

  if (y1 >= 0)
  {
    line = (u16*)(g_currentBuffer + 120 * y0);

    y = y1 - y0;

    u16 tc;

    do 
    {
      s16 clipped_x1 = min(118, x1 >> 17);
      s16 clipped_x0 = min(118, x0 >> 17);

      u16* endx = (u16*)(line + max(0, clipped_x1));
      u16 *curx = (u16*)(line + max(0, clipped_x0));

      tu = u0;
      tv = v0;

      #define PLOT \
      { \
        tc = texture[(((tu >> 8) & 0xFF) + (((tv >> 8) & 0xFF) << 8)) & 0x3FFF] - g_paletteOffset; \
        tu += du; tv += dv; ts += ds; tt += dt; \
        tc |= (texture[(((tu >> 8) & 0xFF) + (((tv >> 8) & 0xFF) << 8)) & 0x3FFF] - g_paletteOffset) << 8; \
        tu += du; tv += dv; ts += ds; tt += dt; \
        *curx++ = tc; \
      }

      stride = endx - curx + 2;
      n = (stride + 7) >> 3;

      switch (stride & 7)
      {
        case 0: do { PLOT;   
        case 7: PLOT;
        case 6: PLOT;
        case 5: PLOT; 
        case 4: PLOT;   
        case 3: PLOT;
        case 2: PLOT;
        case 1: PLOT;  
        } while (--n); 
      }

      x0 += dxAC;
      x1 += dxAB;
      u0 += duAC;
      v0 += dvAC;

      line += 120;

    } while (--y);
  }

  if (flags & SWAPPED)
  {
    x0 = x2;
    u0 = u2;
    v0 = v2;

    u1 += duAB * dy;
    v1 += dvAB * dy;

    dxAC = dxBC;
    duAC = duBC;
    dvAC = dvBC;

    dxBC = dxAB;
    duBC = duAB;
    dvBC = dvAB;
  }
  else
  {
    x1 = x2;
    u1 = u2;
    v1 = v2;
  }
          
  if (y1 < 0)
  {
    if (flags & SWAPPED)
    {
      int overclip = y1;

      x0 -= overclip * dxAC;
      x1 -= (y0 - 0) * dxBC;
      u0 -= overclip * duAC;
      v0 -= overclip * dvAC;
    }
    else
    {
      int overclip = y2;

      x0 -= overclip * dxAC;
      x1 -= (y1 - 0) * dxBC;
      u0 -= overclip * duAC;
      v0 -= overclip * dvAC;
    }

    y1 = 0;
  }

  if (120 * y1 > 120 * 160)
    return;

  line = (u16*)(g_currentBuffer + 120 * y1);

  y = y2 - y1;

  u16 tc;

  do
  {
    s16 clipped_x1 = min(118, x1 >> 17);
    s16 clipped_x0 = min(118, x0 >> 17);
    u16* endx = (u16*)(line + max(0, clipped_x1));
    u16 *curx = (u16*)(line + max(0, clipped_x0));

    tu = u0;
    tv = v0;

    #define PLOT \
    { \
      tc = texture[(((tu >> 8) & 0xFF) + (((tv >> 8) & 0xFF) << 8)) & 0x3FFF] - g_paletteOffset; \
      tu += du; tv += dv; ts += ds; tt += dt; \
      tc |= (texture[(((tu >> 8) & 0xFF) + (((tv >> 8) & 0xFF) << 8)) & 0x3FFF] - g_paletteOffset) << 8; \
      tu += du; tv += dv; ts += ds; tt += dt; \
      *curx++ = tc; \
    }

    stride = endx - curx + 2;
    n = (stride + 7) >> 3;
    
    switch (stride & 7)
    {
      case 0: do { PLOT;   
      case 7: PLOT;
      case 6: PLOT;
      case 5: PLOT; 
      case 4: PLOT;   
      case 3: PLOT;
      case 2: PLOT;
      case 1: PLOT;  
      } while (--n); 
    }
      
    x0 += dxAC;
    x1 += dxBC;
    u0 += duAC;
    v0 += dvAC;

    line += 120;

  } while (--y);
}

void AALine(int x0, int y0, int x1, int y1, u8 color)
{
  u16 *target = (u16*)g_currentBuffer;

  if (y0 > y1) 
  {
    _swap(x0, x1);
    _swap(y0, y1);
  }

  int dx = x1 - x0;
  int dy = y1 - y0;

  int gradient = dx < 0 ? -1 : 1;
  dx *= gradient;

  PutPixel(x0, y0, color);

  if (dy == 0) 
  {
    while (dx--) PutPixel(x0 += gradient, y0, color);
    return;
  }

  if (dx == 0) 
  { 
    while (dy--) PutPixel(x0, y0++, color);
    return;
  }

  if (dx == dy) 
  {
    while (dy--) PutPixel(x0 += gradient, y0++, color);
    return;
  }
  
  u16 currentError;
  u16 error = 0;
  u16 intensity = 0;
   
  if (dy > dx) 
  {
    u16 fract = _div(dx << 16, dy);

    while (--dy) 
    {
      currentError = error;   
      x0 += ((error += fract) <= currentError) ? gradient : 0;

      intensity = error >> 8;

      PutPixel(x0, ++y0, color + intensity);
      PutPixel(x0 + gradient, y0, color + (intensity ^ 255));
    }
  }
  else
  {
    u16 fract = _div(dy << 16, dx);

    while (--dx) 
    {
      currentError = error; 
      y0 += ((error += fract) <= currentError) ? 1 : 0;

      intensity = error >> 8;

      PutPixel(x0 += gradient, y0, color + intensity);
      PutPixel(x0, y0 + 1, color + (intensity ^ 255));
    }
  }
  PutPixel(x1, y1, color);
}

void wireTriangle(KDFace *face, u16 color)
{
  s16 x1 = face->m_vertices[0].m_position.x;
  s16 y1 = face->m_vertices[0].m_position.y;
  s16 x2 = face->m_vertices[1].m_position.x;
  s16 y2 = face->m_vertices[1].m_position.y;
  s16 x3 = face->m_vertices[2].m_position.x;
  s16 y3 = face->m_vertices[2].m_position.y;

  AALine(x1, y1, x2, y2, color);
  AALine(x1, y1, x3, y3, color);
  AALine(x2, y2, x3, y3, color);
}

void SwapFace(KDFace *a, KDFace * b)
{
  KDFace t = *a; 
  *a = *b; 
  *b = t;
}

void IWRAM_CODE SwapIndices(u16 *a, u16 *b, u16 l, u16 r)
{
  u16 t = *a;
  *a = *b;
  *b = t;

  t = drawOrder[l];
  drawOrder[l] = drawOrder[r];
  drawOrder[r] = t;
}

void IWRAM_CODE QuickSort(u16 *list, u16 start, u16 end)
{
  u16 z, l, r;

  if (end > start + 1)
  {
    z = list[start];
    l = start + 1;
    r = end;

    while (l < r)
    {
      if (list[l] > z)
      { 
        l++;
      }
      else
      {
        r--;
        SwapIndices(&list[l], &list[r], l, r);
      }
    }
    l--;
    SwapIndices(&list[l], &list[start], l, start);
    QuickSort(list, start, l);
    QuickSort(list, r, end);
  }
} 

void TransformLocalToWorldProjection(KDModel *model, KDVertex *v, Matrix3x3 m)
{    
  // rotate
  int nx = (m[0][0] * v->m_position.x + m[0][1] * v->m_position.y + m[0][2] * v->m_position.z) >> 8;
  int ny = (m[1][0] * v->m_position.x + m[1][1] * v->m_position.y + m[1][2] * v->m_position.z) >> 8;
  int nz = (m[2][0] * v->m_position.x + m[2][1] * v->m_position.y + m[2][2] * v->m_position.z) >> 8;

  v->m_position.x = nx;
  v->m_position.y = ny;
  v->m_position.z = nz;

#if 1
  // Pseudo-normal approximation looks good enough for convex object
  nx = (nx) + 64;//(_div(nx << 8, len)) + 63;
  ny = (ny >> 1) + 32;//(_div(nx << 8, len)) + 63;
#else
  nx = (nx >> 1) + 64;//(_div(nx << 8, len)) + 63;
  ny = (ny >> 2) + 32;//(_div(nx << 8, len)) + 63;
#endif

  v->s = nx;
  v->t = ny;

  // Translate
  v->m_position.x += model->m_position.x;
  v->m_position.y += model->m_position.y;
  v->m_position.z += model->m_position.z;

  // Project
  
  v->m_position.x = _div(v->m_position.x << 7, v->m_position.z - 64);
  v->m_position.y = _div(v->m_position.y << 7, v->m_position.z - 64);
  
     // v->m_position.x = ((v->m_position.x * (v->m_position.z + 256)) >> 8);
      //v->m_position.y = ((v->m_position.y * (v->m_position.z + 256)) >> 8);
}

void TransformToScreenSpace(KDVertex *v)
{
  v->m_position.x += HALFWIDTH;
  v->m_position.y += HALFHEIGHT;
}

#define LUTSIZE 255
void Render(KDModel *model)
{
  int i, normal;

  Matrix3x3 m;
  Rotate(m, model->m_rotation.x, model->m_rotation.y, model->m_rotation.z);

  int numFaces = 0;
  KDFace *idx = model->m_faces;
  while (idx != 0) 
  {
    KDFace face;
    for (i = 0; i < 3; i++)
    {
     face.m_vertices[i] = idx->m_vertices[i];

     TransformLocalToWorldProjection(model, &face.m_vertices[i], &m);
    }

    // Calculate normal for culling and lighting
    normal = ((face.m_vertices[1].m_position.x - face.m_vertices[0].m_position.x) * 
              (face.m_vertices[2].m_position.y - face.m_vertices[1].m_position.y) - 
              (face.m_vertices[1].m_position.y - face.m_vertices[0].m_position.y) * 
              (face.m_vertices[2].m_position.x - face.m_vertices[1].m_position.x));  

    face.m_normal = normal;

    for (i = 0; i < 3; i++)
    {
      TransformToScreenSpace(&face.m_vertices[i]);
    }

   if (model->m_flags & MODEL_ENVMAPPED)
   {   
      for (i = 0; i < 3; i++)
      {
         face.m_vertices[i].u = idx->m_vertices[i].s;      
         face.m_vertices[i].v = idx->m_vertices[i].t;
      }
   }
   else 
   for (i = 0; i < 3; i++)
   {
      face.m_vertices[i].u = idx->m_vertices[i].u;      
      face.m_vertices[i].v = idx->m_vertices[i].v;
   }

    bool off_screen = 
      (((face.m_vertices[0].m_position.y < 0) &&            (face.m_vertices[1].m_position.y < 0) &&              (face.m_vertices[2].m_position.y < 0)) ||
	    ((face.m_vertices[0].m_position.y > SCREENHEIGHT) && (face.m_vertices[1].m_position.y > SCREENHEIGHT) &&   (face.m_vertices[2].m_position.y > SCREENHEIGHT)) ||
	    ((face.m_vertices[0].m_position.x < 0) &&            (face.m_vertices[1].m_position.x < 0) &&              (face.m_vertices[2].m_position.x < 0)) ||
	    ((face.m_vertices[0].m_position.x > SCREENWIDTH) &&  (face.m_vertices[1].m_position.x > SCREENWIDTH) &&    (face.m_vertices[2].m_position.x > SCREENWIDTH)));

    if (off_screen == false)
    {
/*
      clipInput[0][0] = face.m_vertices[0].m_position.x;
      clipInput[0][1] = face.m_vertices[0].m_position.y;
      clipInput[1][0] = face.m_vertices[1].m_position.x;
      clipInput[1][1] = face.m_vertices[1].m_position.y;
      clipInput[2][0] = face.m_vertices[2].m_position.x;
      clipInput[2][1] = face.m_vertices[2].m_position.y;

      int numVertices = 3;
      numVertices = ClipTop(numVertices, 0);
      numVertices = ClipLeft(numVertices, 0);
      numVertices = ClipBottom(numVertices, SCREENHEIGHT);
      numVertices = ClipRight(numVertices, SCREENWIDTH);

      debugValue = 0;

      switch (numVertices)
      {
         case 4: // insert 2 pre-transformed triangles in transformedFaces array

            //numFaces++;
            //debugValue = 62;
            break;
         case 5:
           // debugValue = 122;
            //numFaces += 2;
            break;
         case 6:
            //debugValue = 190;
            //numFaces += 3;
            break;

         default:
            face.m_vertices[0].m_position.x = clipInput[0][0];
            face.m_vertices[0].m_position.y = clipInput[0][1];
            face.m_vertices[1].m_position.x = clipInput[1][0];
            face.m_vertices[1].m_position.y = clipInput[1][1];
            face.m_vertices[2].m_position.x = clipInput[2][0];
            face.m_vertices[2].m_position.y = clipInput[2][1];
            break;
      }
*/
      // Add visible faces to the list
      if (model->m_flags & MODEL_BACKFACECULLING)
      {     
         if (normal > 0)
         {           
            transformedFaces[numFaces] = face;

            transformedFaces[numFaces].m_next = NULL;    // Redundant?    

            // Calculate depth for sorting based on average Z
            depthBuffer[numFaces] = face.m_vertices[0].m_position.z + face.m_vertices[1].m_position.z + face.m_vertices[2].m_position.z;
            
            numFaces++;
         }
      }
      else
      {
         transformedFaces[numFaces] = face;

         transformedFaces[numFaces].m_next = NULL;         // Redundant?

         // Calculate depth for sorting based on average Z
         depthBuffer[numFaces] = face.m_vertices[0].m_position.z + face.m_vertices[1].m_position.z + face.m_vertices[2].m_position.z;
         
         numFaces++;      
      }
      
      idx = idx->m_next;
   }
  }
  
  // Index triangles
  for (i = 0; i < numFaces; i++)
    drawOrder[i] = i;

  // Rearrange indices based on average depth per triangle
  if (model->m_flags & MODEL_SORTFACES)
  {
    QuickSort(depthBuffer, 0, numFaces);
  }

  u16 faceIndex;

  // Draw back to front
  for (i = 0; i < numFaces; i++) 
  {
    faceIndex = drawOrder[i];

    if (model->m_flags & MODEL_TEXTURED)
    {
      normal = transformedFaces[faceIndex].m_normal >> 2;

      TextureTriangle(&transformedFaces[faceIndex], g_currentTexture, g_currentBumpMap, 0);
		}
    else if (model->m_flags & MODEL_ENVMAPPED)
    {
      normal = transformedFaces[faceIndex].m_normal >> 2;

      TextureTriangle(&transformedFaces[faceIndex], g_currentTexture, g_currentBumpMap, 0);      
    }
    else if (model->m_flags & MODEL_WIREFRAME)
    {
      wireTriangle(&transformedFaces[faceIndex], 0);
    }
    else
    {
      normal = transformedFaces[faceIndex].m_normal >> 2;
    //  normal = depthBuffer[i] >> 3;
      int shade = (((normal >> 2) + depthBuffer[i]) >> 2);
      shade *= shade;
      shade >>= currentModel.m_blend;

     // if (debugValue > 0) shade = debugValue;

     // if (shade < 64 + (SinLUT[64 + model->m_blend] >> 8));
      flatTriangle(&transformedFaces[faceIndex], /*128-normal*/shade);
    }
    
  }
}

void LoadModelFlat(KDModel *model, KDFace f[], const s16 positions[], int numVertices, int scale)
{
  int i, j;
  int k = 0;

  for (i = 0; i < numVertices / 3; i++)
  {
    for (j = 0; j < 3; j++) 
    {
      f[i].m_vertices[j].m_position.x = (int)(positions[k++] * scale) >> 16;
      f[i].m_vertices[j].m_position.y = (int)(positions[k++] * scale) >> 16;
      f[i].m_vertices[j].m_position.z = (int)(positions[k++] * scale) >> 16;   
    }
  }

  model->m_faces = &f[0];
  int numFaces = numVertices / 3;

  for (j = 0; j <numFaces - 1; j++)
  {
      f[j].m_next = &f[j+1];
  }

  f[numFaces-1].m_next = 0;

  model->m_rotation.x = 0;
  model->m_rotation.y = 0;
  model->m_rotation.z = 0;
  model->m_position.x = 0;
  model->m_position.y = 0;
  model->m_position.z = 0;
  model->m_flags = MODEL_BACKFACECULLING | MODEL_SORTFACES;
}

void LoadModel(KDModel *model, KDFace f[], const s16 positions[], const s16 normals[], const u16 texcoords[], int numVertices, int scale, int textured)
{
  int i, j;
  int k = 0, l = 0;

  for (i = 0; i < numVertices / 3; i++)
  {
    for (j = 0; j < 3; j++) 
    {
      f[i].m_vertices[j].m_position.x = (int)(positions[k++] * scale) >> 16;
      f[i].m_vertices[j].m_position.y = (int)(positions[k++] * scale) >> 16;
      f[i].m_vertices[j].m_position.z = (int)(positions[k++] * scale) >> 16;

      f[i].m_vertices[j].u = texcoords[l++];
      f[i].m_vertices[j].v = texcoords[l++];  

      f[i].m_vertices[j].s = (normals[k - 3] + 255) >> 2;
      f[i].m_vertices[j].t = (normals[k - 2] + 255) >> 2;        
    }
  }

  model->m_faces = &f[0];
  int numFaces = numVertices / 3;

  for (j = 0; j <numFaces - 1; j++)
    f[j].m_next = &f[j+1];

  f[numFaces-1].m_next = 0;

  model->m_rotation.x = 0;
  model->m_rotation.y = 0;
  model->m_rotation.z = 0;
  model->m_position.x = 0;
  model->m_position.y = 0;
  model->m_position.z = 0;
  model->m_flags = MODEL_BACKFACECULLING | MODEL_SORTFACES | MODEL_TEXTURED;
}