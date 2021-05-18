// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: Effect_Freedir.h
// Description: Raytracing the intersection with a plane, otherwise known as free-direction planes in the demoscene. 
//              Instead of interpolating between colors this effect interpolates between texture-coordinates, which 
//              results in a sharp image with unique pixel color lookups. Essentially this means that only 15 x 20 
//              grid points have to be raytraced instead of a full-screen image. 
//              
//
// Note: In addition to saving texture-coordinates to a UV table I also save the depth to a shading table so that 
//       I can shift the palette to a darker shade when the distance increases. However, the interpolation reveals
//       artifacts when the delta between values becomes large (e.g. when overshooting max-UV). 
//
// Note: the shading is honestly a bit of a hack. I created a tool in Python that quantizes an indexed color 8-bit
//       bitmap image to 32 colors and create 7 shades of that 32-color palette. This is why g_paletteOffset exists. 

#ifndef __EFFECT_FREEDIR_H
#define __EFFECT_FREEDIR_H

#include "KDTypes.h"

void Freedir_Init();
void Freedir_Destroy();
void Freedir_VCount();
void Freedir_Update(uint time);

#endif//__EFFECT_FREEDIR_H