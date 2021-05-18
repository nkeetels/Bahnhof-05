// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: KDMemory.h
// Description: Contains fast memset and memcpy functions.

#include "KDMemory.h"

void IWRAM_CODE Memcpy32(void *dest, const void* src, unsigned int count)
{
  u32* d = (u32*)dest;
  const u32 *s = (const u32*)src; 

  while (count-=4)
      *d++ = *s++;
}

void IWRAM_CODE DMACopy(void *src, void *dest, u32 count, u32 mode) 
{
  *(volatile u32*)0x40000D4 = (u32)src;
  *(volatile u32*)0x40000D8 = (u32)dest;
  *(volatile u32*)0x40000DC = count | mode;

  while(*(volatile u16*)0x40000DE & 0x8000);
}

void IWRAM_CODE FastCopy16(u16 *dst, const u16 *src, int count)
{
     int n = (count + 7) >> 3;

     switch (count & 7)
     {
     case 0: do {  *dst++ = *src++;
     case 7:       *dst++ = *src++;
     case 6:       *dst++ = *src++;
     case 5:       *dst++ = *src++;
     case 4:       *dst++ = *src++;
     case 3:       *dst++ = *src++;
     case 2:       *dst++ = *src++;
     case 1:       *dst++ = *src++;
     } while (--n > 0); 
     }
}

void IWRAM_CODE FastSet16(u16 *dst, const u16 val, int count)
{
     int n = (count + 7) >> 3;

     switch (count & 7)
     {
     case 0: do {  *dst++ = val;
     case 7:       *dst++ = val;
     case 6:       *dst++ = val;
     case 5:       *dst++ = val;
     case 4:       *dst++ = val;
     case 3:       *dst++ = val;
     case 2:       *dst++ = val;
     case 1:       *dst++ = val;
     } while (--n > 0); 
     }
}
