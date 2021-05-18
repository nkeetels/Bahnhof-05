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
// Note: the bloom effect uses additive feedback, hence the palette should be sorted according to intensity. 

#ifndef __KDPOSTPROCESSING_H
#define __KDPOSTPROCESSING_H

#include "KDTypes.h"

void Post_Bloom(const u8 bloom_bias, const u8 bloom_thredhold);
void Post_Contrast(u16 *dst, const u16 *palette, u8 v);
void VCount_Vignette();
void VCount_Glitch();

#endif//__KDPOSTPROCESSING_H
