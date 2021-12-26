// Draw cube normals > 0
// Draw icosahedron
// Draw cube normals < 0

#include "KDEngine.h"
#include "KDMesh.h"
#include "Effect_SphereDistort.h"
#include "KDPostProcessing.h"

KDVertex cube[8];

void SphereDistort_Init()
{
  OAM_Clear();
  OAM_CopySprites();
  
  REG_DISPCNT = 0x1444;
  for (int i = 0; i < 256; i++)
  { 
    ((unsigned short*)0x5000000)[i] = RGB15(i,i,i);
  }
}

void SphereDistort_Destroy()
{

}

#define RENDER_INSIDE 1 << 1
#define RENDER_OUTSIDE 1 << 2
void Render_Cube(KDModel *model, Matrix3x3 m, u8 cullingFlags)
{
  for (int i = 0; i < 8; i++)
  {
    TransformLocalToWorldProjection(model, &cube[i], m);
    TransformToScreenSpace(&cube[i]);
  }
 
  AALine(cube[0].m_position.x, cube[0].m_position.y, cube[1].m_position.x, cube[1].m_position.y, 0);
  AALine(cube[1].m_position.x, cube[1].m_position.y, cube[2].m_position.x, cube[2].m_position.y, 0);
  AALine(cube[2].m_position.x, cube[2].m_position.y, cube[3].m_position.x, cube[3].m_position.y, 0);
  AALine(cube[3].m_position.x, cube[3].m_position.y, cube[0].m_position.x, cube[0].m_position.y, 0);

  AALine(cube[4].m_position.x, cube[4].m_position.y, cube[5].m_position.x, cube[5].m_position.y, 0);
  AALine(cube[5].m_position.x, cube[5].m_position.y, cube[6].m_position.x, cube[6].m_position.y, 0);
  AALine(cube[6].m_position.x, cube[6].m_position.y, cube[7].m_position.x, cube[7].m_position.y, 0);
  AALine(cube[7].m_position.x, cube[7].m_position.y, cube[4].m_position.x, cube[4].m_position.y, 0);

  AALine(cube[0].m_position.x, cube[0].m_position.y, cube[4].m_position.x, cube[4].m_position.y, 0);
  AALine(cube[3].m_position.x, cube[3].m_position.y, cube[7].m_position.x, cube[7].m_position.y, 0);
  AALine(cube[1].m_position.x, cube[1].m_position.y, cube[5].m_position.x, cube[5].m_position.y, 0);
  AALine(cube[2].m_position.x, cube[2].m_position.y, cube[6].m_position.x, cube[6].m_position.y, 0);
}


void Generate_Cube()
{
  int size = 65;

  cube[0].m_position.x = -size;
  cube[0].m_position.y = -size; 
  cube[0].m_position.z = -size;
  cube[1].m_position.x = size;
  cube[1].m_position.y = -size; 
  cube[1].m_position.z = -size;
  cube[2].m_position.x = size;
  cube[2].m_position.y = size; 
  cube[2].m_position.z = -size;
  cube[3].m_position.x = -size;
  cube[3].m_position.y = size; 
  cube[3].m_position.z = -size;
  cube[4].m_position.x = -size;
  cube[4].m_position.y = -size; 
  cube[4].m_position.z = size;
  cube[5].m_position.x = size;
  cube[5].m_position.y = -size; 
  cube[5].m_position.z = size;
  cube[6].m_position.x = size;
  cube[6].m_position.y = size; 
  cube[6].m_position.z = size;
  cube[7].m_position.x = -size;
  cube[7].m_position.y = size; 
  cube[7].m_position.z = size;
}

void SphereDistort_Update(uint time)
{
  FastSet16(g_currentBuffer, (255 << 8) | 255, (SCREENWIDTH*SCREENHEIGHT)>>1);  

  Icosahedron_Subdivide(time, MESH_RESTRICTED);  

  currentModel.m_position.z = -160;
  currentModel.m_rotation.x = -time>>2;
  currentModel.m_rotation.y = -time>>1;
  currentModel.m_rotation.z = -time>>2;
  currentModel.m_blend = 15;
  currentModel.m_flags = MODEL_SORTFACES | MODEL_BACKFACECULLING/* | MODEL_WIREFRAME*/;  

  Render(&currentModel);  

  
  Matrix3x3 m;
  Rotate(m, currentModel.m_rotation.x, currentModel.m_rotation.y, currentModel.m_rotation.z);
  Generate_Cube();

  Render_Cube(&currentModel, m, RENDER_INSIDE);

}

void SphereDistort_VCount()
{
  //VCount_Glitch();
}