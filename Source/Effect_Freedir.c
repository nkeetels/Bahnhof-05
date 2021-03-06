
#include "KDEngine.h"
#include "KDPostProcessing.h"
#include "Effect_Freedir.h"

#define PLANES
//#define TUNNEL
//#define SPHERE
#define PRECISION

#define tab 45

int u, v, z;

extern const unsigned short metalBitmap[8192];
extern const unsigned short metalPal[256];
extern const unsigned char splat32Bitmap[8192];
extern const unsigned short splat32Pal[256];

extern const unsigned short bahnhof1Tiles[2048];
extern const unsigned short bahnhof2Tiles[2048];
extern const unsigned short bahnhof3Tiles[2048];
extern const unsigned short bahnhof4Tiles[2048];
extern const unsigned short bahnhof1Pal[256];

u8 uvtable[20 * 15 * 3];
u8 EWRAM_DATA shadingtable[20 * 15];

void Freedir_Init()
{
  REG_DISPCNT = 0x1444;

  int i;
  for (i = 0; i < 256; i++)
  {
    u8 r = (metalPal[i] & 0x7C00) >> 10;
    u8 g = (metalPal[i] & 0x3E0) >> 5;
    u8 b = (metalPal[i] & 0x1F);

    r >>= 1;
    g >>= 1;
    b >>= 1;

    ((unsigned short*)0x5000000)[i] = (r << 10) | (r << 5) | g;
  }  

  //OAM_Overlay(bahnhof1Pal, bahnhof1Tiles, bahnhof2Tiles, bahnhof3Tiles, bahnhof4Tiles);
}

void Freedir_VCount()
{
}

void Freedir_Destroy()
{
  REG_BG2PA = 256;
  REG_BG2PB = 0;
  REG_BG2PC = 0;
  REG_BG2PD = 256;
}

int sgn(int x)
{
    return (x > 0) - (x < 0);
}

void RotatePalette(uint time)
{ 
  int i;
  for (i = 0; i < 256; i++)
  {
    u8 r = (metalPal[i] & 0x7C00) >> 10;
    u8 g = (metalPal[i] & 0x3E0) >> 5;
    u8 b = (metalPal[i] & 0x1F);

    r += (SinLUT[time>>1] >> 2);
    g += (SinLUT[time>>1] >> 2);
    b += (SinLUT[time>>1] >> 2);

    ((unsigned short*)0x5000000)[i] = (b << 10) | (g << 5) | r;
  }

}

#define ROTOZOOM_BOUNCE 1 << 1

void IWRAM_CODE Freedir_Update(uint time)
{
  const u8 palEntry = 139;
  const u16 bgColor = (palEntry << 8) | palEntry;;
  FastSet16((u16*)g_currentBuffer, bgColor, 160*120);    

    int bounce = 0;

    Matrix3x3 m;

    Rotate(m, time>>1, 64 + ((time-300)), time>>2);
    //Rotate(m, time>>3, 0, time>>4);
 
    u16 *target32 = g_currentBuffer;

    int y;
    int i, j;
    int uv, offset = 0, voffset = 0;
    int ox, oy, oz;
    int dx, dy, dz;
    int nx, ny, nz;

    int ix, iz, iy, t, ilen, color;

    u8 *tex = (u8*)metalBitmap;
    
#ifdef PLANES   
        ox = 0;//time<<1;//(SinLUT[((time >> 1) + 64)&255]) >> 1;
        oy = 0;//(SinLUT[(time >> 1)&255]) << 4;
        oz = 0;//time>>1;//(SinLUT[((time >> 1))&255]) >> 1;                    
#endif            
#ifdef TUNNEL
        ox = -128;
        oy = 128;
        oz = 0;//time << 10;    
#endif
#ifdef SPHERE
        ox = -128;
        oy = 128;
        oz = 0;//time << 10;      
#endif

    j = 0;
  
    do 
    {
        i = 0;
        do
        {                                        
            dx = (((i<<1) - 120) << 8) >> 7;
            dy = ((j - 80) << 8) >> 7;
            dz = 1 << 8;

            nx = (m[0][0] * dx + m[0][1] * dy + m[0][2] * dz) >> 8;
            ny = (m[1][0] * dx + m[1][1] * dy + m[1][2] * dz) >> 8;
            nz = (m[2][0] * dx + m[2][1] * dy + m[2][2] * dz) >> 8;

            ilen = _sqrt(nx * nx + ny * ny + nz * nz);

            nx = _div(nx << 8, ilen);
            ny = _div(ny << 8, ilen);
            nz = _div(nz << 8, ilen);

            dx = (nx * ilen) >> 8;
            dy = (ny * ilen) >> 8;
            dz = (nz * ilen) >> 8;

#ifdef PLANES
            if (dy == 0)
                t = 65535;
            else        
                t = _div((sgn(dy) << 13) - oy, dy) + bounce;

            ix = ox + ((int)(dx * t) >> 8);
            iz = oz + ((int)(dz * t) >> 8);

            u = (int)(abs(ix)<<1) + 64;
            v = (int)(abs(iz)<<1) + 64;

            t >>= 6;
#endif            
#ifdef TUNNEL
            int a = dx * dx + dy * dy;
            int b = 2 * (ox * dx + oy * dy);
            int c = ox * ox + oy * oy - 256 * 256;

            int delta = _sqrt(b * b - 4 * a * c);
            int t1 = _div((-b + delta) << 8, 2 * a);
            int t2 = _div((-b - delta) << 8, 2 * a);

            t = min(t1, t2);                

            //t = t1;//1 << 6;

            ix = ox + (dx * t) >> 8;
            iy = oy + (dy * t) >> 8;
            iz = oz + (dz * t) >> 8;

            u = (int)(abs(iz));
            v = (int)((abs((int)(_atan2(iy, ix)))) >> 6);

            t = abs(max(t1, t2)) >> 7;
#endif
#ifdef SPHERE
            int a = dx * dx + dy * dy + dz * dz;
            int b = 2 * (ox * dx + oy * dy + oz * dz);
            int c = ox * ox + oy * oy + oz * oz - 256 * 256;

            int delta = _sqrt(b * b - 4 * a * c);
            int t1 = _div((-b + delta) << 8, 2 * a);
            int t2 = _div((-b - delta) << 8, 2 * a);

            t = min(t1, t2);                

            ix = ox + (dx * t) >> 8;
            iy = oy + (dy * t) >> 8;
            iz = oz + (dz * t) >> 8;

            u = (iy * iy + iz * iz) >> 8;
            v = (int)((abs((int)(_atan2(iz, ix)))) >> 6);

            t = 1;
#endif
            t = min(t, 7);

            uvtable[offset++] = u;
            uvtable[offset++] = v;
            uvtable[offset++] = t << 5;

            voffset++;
            i += 8;
        } while (i < 120);
        voffset+=120;
        j += 8;
    } while (j < 160);

    int u1, v1, u2, v2, u3, v3, u4, v4;
    int s1, t1, s2, t2, texofs = 0;

    offset = 0, y = 0;
    int ymul = 0;
    int du1, du2, dv1, dv2, eu1, eu2, d1, d2, d3, d4, zdu1, zdu2;
    int tempu1, tempu2, tempv1, tempv2, tempz1, tempz2 ,z1, z2, z, vz1;

    u8 *uvs = (u8*)uvtable;

   for (j = 1; j < 19; j++)
    {
        for (i = 1; i < 15; i++) 
        {   
            u1 = *uvs++;
            v1 = *uvs++;
            d1 = *uvs++;

            u2 = *uvs++;
            v2 = *uvs++;
            d2 = *uvs++;

            uvs += tab - 6;

            u3 = *uvs++;
            v3 = *uvs++;
            d3 = *uvs++;

            u4 = *uvs++;
            v4 = *uvs++;
            d4 = *uvs++;

            uvs -= tab;
            uvs -= 3;

#ifdef PRECISION
            du1 = (((u2 - u1) << 16) >> 3);
            du2 = (((u4 - u3) << 16) >> 3);
            dv1 = (((v2 - v1) << 16) >> 3);
            dv2 = (((v4 - v3) << 16) >> 3);

            zdu1 = (((d2 - d1) << 16) >> 3);
            zdu2 = (((d4 - d3) << 16) >> 3);
#else
            du1 = (((u2 - u1)) >> 3);
            du2 = (((u4 - u3)) >> 3);
            dv1 = (((v2 - v1)) >> 3);
            dv2 = (((v4 - v3)) >> 3);

            zdu1 = (((d2 - d1)) >> 3);
            zdu2 = (((d4 - d3)) >> 3);
#endif

            ymul = (((j << 3)) * 120) + (i << 3);
            for (y = 0; y < 8; y += 2)
            {
                tempu1 = tempu2 = tempv1 = tempv2 = tempz1 = tempz2 = 0;
                {     
#ifdef PRECISION
                    #define PLOT(add) \
                    { \  
                      tempu1 += du1; tempu2 += du2; s1 = u1 + (tempu1 >> 16); s2 = u3 + (tempu2 >> 16); \
                      tempv1 += dv1; tempv2 += dv2; t1 = v1 + (tempv1 >> 16); t2 = v3 + (tempv2 >> 16); \
                      tempz1 += zdu1; tempz2 += zdu2; z1 = d1 + (tempz1 >> 16); z2 = d3 + (tempz2 >> 16);\
                      vz1 = (((z2 - z1) << 16) >> 3); \
                      eu1 = (((s2 - s1) << 16) >> 3); eu2 = (((t2 - t1) << 16) >> 3); \
                      u = (s1 + ((eu1 * y) >> 16)); v = (t1 + ((eu2 * y) >> 16)); z = z1 + ((vz1 * y) >> 16) >> 5; \  
                      offset = ymul + add; \   
                      texofs = ((v << 7) + u) & 0x3FFF; color = tex[texofs] - 224 + (z << 5); target32[offset] = (color << 8) | color; \       
                      target32[offset + 120] = (color << 8) | color; \     
                    }   
#else                        
                    #define PLOT(add) \
                    { \  
                    tempu1 += du1; tempu2 += du2; s1 = u1 + (tempu1); s2 = u3 + (tempu2); \
                    tempv1 += dv1; tempv2 += dv2; t1 = v1 + (tempv1); t2 = v3 + (tempv2); \
                    tempz1 += zdu1; tempz2 += zdu2; z1 = d1 + (tempz1); z2 = d3 + (tempz2);\
                    vz1 = (((z2 - z1)) >> 3); \
                    eu1 = (((s2 - s1)) >> 3); eu2 = (((t2 - t1)) >> 3); \
                    u = (s1 + ((eu1 * y))); v = (t1 + ((eu2 * y))); z = z1 + ((vz1 * y)) >> 5; \  
                    offset = ymul + add; \   
                    texofs = ((v << 7) + u) & 0x3FFF; color = tex[texofs] - 224 + (z << 5); target32[offset] = (color << 8) | color; \  
                    target32[offset + 120] = (color << 8) | color; \        
                    }                                                               
#endif

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
        uvs += 3;
    }   
}

