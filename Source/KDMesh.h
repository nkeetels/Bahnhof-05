
#ifndef __KDMESH_H
#define __KDMESH_H

#include "KDTypes.h"
#include "KDDefinitions.h"

#define MESH_STATIC 1 << 1
#define MESH_VERTEXDISPLACE 1 << 2
#define MESH_FACEDISPLACE 1 << 3
#define MESH_RESTRICTED 1 << 4

void Icosahedron_Generate(int time);
void Icosahedron_Subdivide(int time, u8 mMode);
void Icosahedron_CreateIndexBuffer(const int numFaces);

#endif//__KDMESH_H