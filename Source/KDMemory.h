// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDMemory.h
// Description: Contains fast memset and memcpy functions.

#ifndef __KDMEMORY_H
#define __KDMEMORY_H

#include "KDTypes.h"
#include "KDDefinitions.h"

void Memcpy32(void *dest, const void* src, unsigned int count);
void DMACopy(void *src, void *dest, u32 count, u32 mode);
void FastCopy16(u16 *dst, const u16 *src, int count);
void FastSet16(u16 *dst, const u16 val, int count);

#endif//__KDMEMORY_H