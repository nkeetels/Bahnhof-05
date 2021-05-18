// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDMAth.h
// Description: defines math types, macros and functions.

#ifndef __KDMATH_H
#define __KDMATH_H

#include "KDTypes.h"

typedef struct VECTOR3
{
  s16 x, y, z;
} Vector3;

typedef struct VECTOR2
{
  s16 x, y;
} Vector2;

typedef int Matrix3x3[3][3];

#define LUTSIZE 255
extern const s16 SinLUT[];

extern int IWRAM_CODE _div(int a, int b);
extern int IWRAM_CODE _atan2(int a, int b);
extern int IWRAM_CODE _sqrt(int a);
extern int IWRAM_CODE swapxy(int a, int b);

#define lerp(a, b, t) ((a * (255 - t) + b * t) >> 8)
#define abs(a) ((a) < (0) ? (-a) : (a))
#define max(a, b) a - ((a-b) & (a-b)>>31)
#define min(a, b) ((((a)-(b)) & 0x80000000) >> 31) ? (a) : (b)
#define clamp(x, a, b) (max(a, min(x, b)))
#define mod(a, b) (a) >= (b) ? (a) % (b) : (a)
#define _swap(x,y) x^=y;y^=x;x^=y

void Rotate(Matrix3x3 m, int x, int y, int z);

#endif//__KDMATH_H
