

#include "KDEngine.h"
#include "KDRaycast.h"
#include "KDEngine.h"

extern const unsigned short gba_labelBitmap[19200];
extern const unsigned short gba_labelPal[256];

extern const unsigned short tiling32Bitmap[8192];
extern const unsigned short tiling32Pal[256];

void KDRaycast_Init()
{
    REG_DISPCNT = 0x1444;    
    FastCopy16(((unsigned short*)0x5000000),gba_labelPal, 256);

    OAM_Clear();

    u8 spriteIndex = 0;
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 14; i++)
        {
            u8 transparancy = 0;
            OAM_SetSprite(spriteIndex++, i * 17, 16 + j * 17, 16, 0, transparancy, tiling32Bitmap, tiling32Pal);
        }
    }

    OAM_CopySprites();
}

void KDRaycast_Destroy()
{
}

void KDRaycast_Update(uint time)
{
 // Memcpy32(g_currentBuffer, gba_labelBitmap, 120*160*2);
}


int zoomin = 512;
void KDRaycast_VCount(int param)
{
}
