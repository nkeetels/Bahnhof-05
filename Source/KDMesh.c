#include "KDEngine.h"
#include "KDMesh.h"

const s16 X = 64 * .525731112119133606f;
const s16 Z = 64 * .850650808352039932f;
const s16 N = 0.f;

const Vector3 ico_verts[] = 
{
  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z}, {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X}, {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
};

const u8 ico_indices[] = 
{
  0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
};

void Icosahedron_CreateIndexBuffer(const int numFaces)
{
  currentModel.m_faces = &currentFaces[0];
  for (int i = 0; i < numFaces - 1; i++)
  {
    currentFaces[i].m_next = &currentFaces[i+1];
  }
    
  currentModel.m_faces[numFaces - 1].m_next = 0;
}

void Icosahedron_Generate(int time)
{
  int index = 0;
  int i = 0;

  for (i = 0; i < 20; i++)
  {
    currentFaces[i].m_vertices[0].m_position.x = ico_verts[ico_indices[index + 0]].x;
    currentFaces[i].m_vertices[0].m_position.y = ico_verts[ico_indices[index + 0]].y;
    currentFaces[i].m_vertices[0].m_position.z = ico_verts[ico_indices[index + 0]].z;
    currentFaces[i].m_vertices[1].m_position.x = ico_verts[ico_indices[index + 1]].x;
    currentFaces[i].m_vertices[1].m_position.y = ico_verts[ico_indices[index + 1]].y;
    currentFaces[i].m_vertices[1].m_position.z = ico_verts[ico_indices[index + 1]].z;
    currentFaces[i].m_vertices[2].m_position.x = ico_verts[ico_indices[index + 2]].x;
    currentFaces[i].m_vertices[2].m_position.y = ico_verts[ico_indices[index + 2]].y;
    currentFaces[i].m_vertices[2].m_position.z = ico_verts[ico_indices[index + 2]].z;

    index += 3;
  }

  Icosahedron_CreateIndexBuffer(20);
}

void Icosahedron_DisplaceFace(KDFace *in, uint value)
{
  s16 nx = in->m_vertices[0].m_position.y * in->m_vertices[1].m_position.z - in->m_vertices[0].m_position.z * in->m_vertices[1].m_position.y;
  s16 ny = in->m_vertices[0].m_position.z * in->m_vertices[1].m_position.x - in->m_vertices[0].m_position.x * in->m_vertices[1].m_position.z;
  s16 nz = in->m_vertices[0].m_position.x * in->m_vertices[1].m_position.y - in->m_vertices[0].m_position.y * in->m_vertices[1].m_position.x;

  s16 h = in->m_vertices[0].m_position.y + in->m_vertices[1].m_position.y + in->m_vertices[2].m_position.y;
  h >>= 3;
  h = abs(SinLUT[(h + value) & 255]);

  s16 speed = 1;

  s16 scale = SinLUT[(in->m_vertices[0].m_position.x * speed + value) & 255] +
              SinLUT[(in->m_vertices[0].m_position.y * speed + (value>>1)) & 255] +
              SinLUT[(in->m_vertices[0].m_position.z * speed + (value>>1)) & 255];

  in->m_vertices[0].m_position.x += nx * scale >> 14; 
  in->m_vertices[0].m_position.z += nz * scale >> 14; 

  in->m_vertices[1].m_position.x += nz * scale >> 14; 
  in->m_vertices[1].m_position.z += nz * scale >> 14; 

  in->m_vertices[2].m_position.x += nx * scale >> 14; 
  in->m_vertices[2].m_position.z += nz * scale >> 14;
}

void Icosahedron_DisplaceVertex(KDVertex *in, uint value)
{
  int x = in->m_position.x;
  int y = in->m_position.y;
  int z = in->m_position.z;

/* // deze is mooi met flat shading
  int rep = 1;
	int scale = SinLUT[((x * rep) + (value >> 0)) & 255] +
					    SinLUT[((y * rep) + (value << 1)) & 255] +
					    SinLUT[((z * rep) + (value >> 1)) & 255];
*/
  int rep = 2;
	int scale = SinLUT[((x * rep) + (value >> 0)) & 255] +
					    SinLUT[((y * rep) + (value << 1)) & 255] +
					    SinLUT[((z * rep) + (value >> 1)) & 255];

  in->m_position.x += (in->m_position.x) * (scale >> 4) >> 5; 
  in->m_position.y += (in->m_position.y) * (scale >> 4) >> 5; 
  in->m_position.z += (in->m_position.z) * (scale >> 4) >> 5; 
}

void Icosahedron_DisplaceVertexRestricted(KDVertex *in, uint value)
{
  int x = in->m_position.x;
  int y = in->m_position.y;
  int z = in->m_position.z;

  int rep = 2;
	int scale = SinLUT[((x * rep) + (value >> 0)) & 255] +
					    SinLUT[((y * rep) + (value << 1)) & 255] +
					    SinLUT[((z * rep) + (value >> 1)) & 255];

  in->m_position.x += (in->m_position.x) * (scale >> 4) >> 5; 
  in->m_position.y += (in->m_position.y) * (scale >> 4) >> 5; 
  in->m_position.z += (in->m_position.z) * (scale >> 4) >> 5; 

  in->m_position.x = clamp(in->m_position.x, -64, 64);
  in->m_position.y = clamp(in->m_position.y, -64, 64);
  in->m_position.z = clamp(in->m_position.z, -64, 64);
}

void Icosahedron_DisplaceVertices(KDFace *in, u16 time)
{
    Icosahedron_DisplaceVertex(&in->m_vertices[0], time);
    Icosahedron_DisplaceVertex(&in->m_vertices[1], time);
    Icosahedron_DisplaceVertex(&in->m_vertices[2], time);   
}

void Icosahedron_DisplaceVerticesRestricted(KDFace *in, u16 time)
{
    Icosahedron_DisplaceVertexRestricted(&in->m_vertices[0], time);
    Icosahedron_DisplaceVertexRestricted(&in->m_vertices[1], time);
    Icosahedron_DisplaceVertexRestricted(&in->m_vertices[2], time);   
}

void Icosahedron_DisplaceNone(KDFace *in, u16 time)
{
}

void Icosahedron_Subdivide(int time, u8 mode)
{
  int i = 0, j = 0, index = 0;
  Vector3 edge1, edge2, edge3;

  void (*displacmentFunc)(KDFace*, uint);

  switch (mode)
  {
    case MESH_FACEDISPLACE:
      displacmentFunc = Icosahedron_DisplaceFace;
      break;
    case MESH_VERTEXDISPLACE:
      displacmentFunc = Icosahedron_DisplaceVertices;
      break;
    case MESH_RESTRICTED:
      displacmentFunc = Icosahedron_DisplaceVerticesRestricted;
      break;
    case MESH_STATIC:
    default:
      displacmentFunc = Icosahedron_DisplaceNone;
  }

  for (i = 0; i < 20; i++)
  {
    s16 t0x0 = ico_verts[ico_indices[index + 0]].x;
    s16 t0y0 = ico_verts[ico_indices[index + 0]].y;
    s16 t0z0 = ico_verts[ico_indices[index + 0]].z;

    s16 t0x1 = ico_verts[ico_indices[index + 1]].x;
    s16 t0y1 = ico_verts[ico_indices[index + 1]].y;
    s16 t0z1 = ico_verts[ico_indices[index + 1]].z;

    s16 t0x2 = ico_verts[ico_indices[index + 2]].x;
    s16 t0y2 = ico_verts[ico_indices[index + 2]].y;
    s16 t0z2 = ico_verts[ico_indices[index + 2]].z;
  
    edge1.x = t0x0 + ((t0x1 - t0x0) >> 1);
    edge1.y = t0y0 + ((t0y1 - t0y0) >> 1);
    edge1.z = t0z0 + ((t0z1 - t0z0) >> 1);

    edge2.x = t0x1 + ((t0x2 - t0x1) >> 1);
    edge2.y = t0y1 + ((t0y2 - t0y1) >> 1);
    edge2.z = t0z1 + ((t0z2 - t0z1) >> 1);

    edge3.x = t0x2 + ((t0x0 - t0x2) >> 1);
    edge3.y = t0y2 + ((t0y0 - t0y2) >> 1);
    edge3.z = t0z2 + ((t0z0 - t0z2) >> 1);

    currentFaces[j].m_vertices[0].m_position.x = edge3.x;
    currentFaces[j].m_vertices[0].m_position.y = edge3.y;
    currentFaces[j].m_vertices[0].m_position.z = edge3.z;
    currentFaces[j].m_vertices[1].m_position.x = t0x0;
    currentFaces[j].m_vertices[1].m_position.y = t0y0;
    currentFaces[j].m_vertices[1].m_position.z = t0z0;
    currentFaces[j].m_vertices[2].m_position.x = edge1.x;
    currentFaces[j].m_vertices[2].m_position.y = edge1.y;
    currentFaces[j].m_vertices[2].m_position.z = edge1.z;
    displacmentFunc(&currentFaces[j], time);

    j++;

    currentFaces[j].m_vertices[0].m_position.x = edge1.x;
    currentFaces[j].m_vertices[0].m_position.y = edge1.y;
    currentFaces[j].m_vertices[0].m_position.z = edge1.z;
    currentFaces[j].m_vertices[1].m_position.x = t0x1;
    currentFaces[j].m_vertices[1].m_position.y = t0y1;
    currentFaces[j].m_vertices[1].m_position.z = t0z1;
    currentFaces[j].m_vertices[2].m_position.x = edge2.x;
    currentFaces[j].m_vertices[2].m_position.y = edge2.y;
    currentFaces[j].m_vertices[2].m_position.z = edge2.z;
    displacmentFunc(&currentFaces[j], time);

    j++;

    currentFaces[j].m_vertices[0].m_position.x = edge2.x;
    currentFaces[j].m_vertices[0].m_position.y = edge2.y;
    currentFaces[j].m_vertices[0].m_position.z = edge2.z;
    currentFaces[j].m_vertices[1].m_position.x = t0x2;
    currentFaces[j].m_vertices[1].m_position.y = t0y2;
    currentFaces[j].m_vertices[1].m_position.z = t0z2;
    currentFaces[j].m_vertices[2].m_position.x = edge3.x;
    currentFaces[j].m_vertices[2].m_position.y = edge3.y;
    currentFaces[j].m_vertices[2].m_position.z = edge3.z;
    displacmentFunc(&currentFaces[j], time);

    j++;

    currentFaces[j].m_vertices[0].m_position.x = edge1.x;
    currentFaces[j].m_vertices[0].m_position.y = edge1.y;
    currentFaces[j].m_vertices[0].m_position.z = edge1.z;
    currentFaces[j].m_vertices[1].m_position.x = edge2.x;
    currentFaces[j].m_vertices[1].m_position.y = edge2.y;
    currentFaces[j].m_vertices[1].m_position.z = edge2.z;
    currentFaces[j].m_vertices[2].m_position.x = edge3.x;
    currentFaces[j].m_vertices[2].m_position.y = edge3.y;
    currentFaces[j].m_vertices[2].m_position.z = edge3.z;
    displacmentFunc(&currentFaces[j], time);

    j++;
    
    index += 3;
  }

  Icosahedron_CreateIndexBuffer(79+1);
}