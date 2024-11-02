#ifndef MINIMAP_H
#define MINIMAP_h

#include "raylib.h"

Vector2 minimapPosition();
int minimapVisibleTilesH;
int minimapVisibleTilesV;
float minimapHeight;

void InitMinimap(Vector3 playerWorldPosition);


#endif