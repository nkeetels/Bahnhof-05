// Button Bashers Secret Santa 2019 Gameboy Advance demo source code
// 
// File: Main.c
// Description: the entry point of the demo, handles initialization of Pimp Mobile module player, sets up 
//              interrupts and features some interrupt-based post-processing effects and a very basic script 
//              system for running the effects.
// 
// Note: because of time pressure there are a couple of hacks in the main-loop of the script system to start / end
//       fades, and I would rather add interrupt support for effects next time so that I can move the code for Plasma
//       to the appropriate effect source file (Intro.c)

#pragma long_calls

#include "KDEngine.h"
#include "FlagEffect.h"
#include "Splash.h"
#include "Intro.h" 
#include "Zoomblur.h"
#include "OBJ_test.h"
#include "KDRaycast.h"
#include "Effect_Icosahedron.h"
#include "Effect_Greetings.h"
#include "Effect_Freedir.h"
#include "Effect_Movetable.h"

#include <gba_base.h>
#include <gba_systemcalls.h>
#include <gba_dma.h>
#include <gba_sound.h>
#include <gba_timers.h>
#include <gba_interrupt.h>

#include "pimpmobile.h"

extern const unsigned char bibntuckerxm[];
extern const unsigned char bib_sample_bank[];

static uint g_frame = 0;
static uint g_currentEffect = 0;
u8 curBlend = 0;
u8 fade =  16;
int scanlineCounter = 0;
int scanline_y;

void VCount_Dummy()
{}

const static void (*g_initializers[])() = 
{  
  MovetableEffect_Init,   
  Icosahedron_Init,  
  Freedir_Init,  
  Greetings_Init, 
  Freedir_Init,  
  /*
  OBJ_Test_Init,
  //FlagEffect_Init,
  //MovetableEffect_Init,    
  
  Splash_Init,
  Intro_Init,
  OBJ_Test_Init, 
  MovetableEffect_Init,   
*/        
};

const static void (*g_effects[])(uint) = 
{ 
  MovetableEffect_Update,   
  Icosahedron_Update,  
  Freedir_Update,  
  Greetings_Update,   
  Freedir_Update,     
  /*
  OBJ_Test_Update,
  //FlagEffect_Update, 
  //MovetableEffect_Update,     
 
  Splash_Update,
  Intro_Update,
  OBJ_Test_Update, 
  MovetableEffect_Update,       
  */  
};

const static void (*g_vcounts[])(uint) = 
{ 
  MovetableEffect_VCount,   
  Icosahedron_VCount,  
  KDRaycast_VCount, 
  VCount_Dummy,
  VCount_Dummy,
  /*
  VCount_Dummy,
  VCount_Dummy,
  VCount_Dummy,
  VCount_Dummy,
  */
};

const static void (*g_destroyers[])() = 
{  

  MovetableEffect_Destroy,   
  Icosahedron_Destroy,  
  Freedir_Destroy,  
  Greetings_Destroy, 
  Freedir_Destroy,  
  /*
  Credits_Destroy,
  Freedir_Destroy,   
  Icosahedron_Destroy,
  MovetableEffect_Destroy,    
  OBJ_Test_Destroy,
  //FlagEffect_Destroy,
        
  Splash_Destroy,
  Intro_Destroy,  
  Freedir_Destroy,    
  OBJ_Test_Destroy,
  MovetableEffect_Destroy,   
  */   
};


const int patternOffset = 70;
const int patternLength = (512 * 2 - patternOffset) >> 1;
const static int transitions[] = 
{
  0, patternLength, 2 * patternLength, 3 * patternLength, 4 * patternLength, 5 * patternLength
};

void vblank()
{
	pimp_vblank();  
	pimp_frame();
  g_frame++;
}

void Fade(int t)
{
  REG_BLDCNT = 0x00ff | ((0x0010)&63);
  REG_BLDY = t;
}

void vcount()
{
  g_vcounts[g_currentEffect](g_frame);  
  scanlineCounter++;
}

void SwapBuffers()
{
    if (REG_DISPCNT & 0x10)
    {
      REG_DISPCNT &= ~0x10;
      g_currentBuffer = BACKBUFFER;
    }
    else
    {
      REG_DISPCNT |= 0x10;
      g_currentBuffer = FRONTBUFFER;
    }  
}

int main()
{
  pimp_init(bibntuckerxm, bib_sample_bank);
	irqInit();
	irqEnable(IRQ_VBLANK | IRQ_HBLANK);
	irqSet(IRQ_VBLANK, vblank);
  irqSet(IRQ_HBLANK, vcount);

  int i = 0;
  while (1)
  {
    g_currentEffect = i;

    g_initializers[i]();

    while (g_frame < transitions[i+1]) 
    {    
      VBlankIntrWait(); 

      g_effects[i](g_frame - transitions[i]);
 
      SwapBuffers();
    }

    g_destroyers[i]();
    i++;
  }
}