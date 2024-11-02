#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "raylib.h"

#define FOV_ANGLE (60 * (PI / 180))
#define NUM_RAYS 800 //this sucks TODO: make it not suck

struct CRay{
    float rayAngle;
    Vector2 wallHit;
    float distance;
    bool isHitVertical;
    bool isRayFacingUp;
    bool isRayFacingDown; 
    bool isRayFacingRight; 
    bool isRayFacingLeft; 
    int wallTileId;
} rays[NUM_RAYS];

static float normalizeRayAngle(float rayAngle);
void castAllRays(Vector3 playerWorldPosition, float playerAngle);
void castRay(float rayAngle, int rayIndex, Vector3 playerWorldPosition);
//void drawRay(float rayAngle, int rayIndex, float lenght);
#endif