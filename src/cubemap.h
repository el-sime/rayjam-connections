#ifndef CUBEMAP_H
#define CUBEMAP_H
#include "raylib.h"

#define COLOR_EQUAL(col1, col2) ((col1.r == col2.r)&&(col1.g == col2.g)&&(col1.b == col2.b)&&(col1.a == col2.a))


// Generate a cubes mesh from pixel data
// NOTE: Vertex data is uploaded to GPU
Mesh GenMeshCubicmapMulticolor(Image cubicmap, Vector3 cubeSize, Color floorColor, Color emptyColor);
#endif