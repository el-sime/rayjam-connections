#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "raylib.h"

#define FOV_ANGLE (60 * (PI / 180))
#define NUM_RAYS 800 //this sucks TODO: make it not suck

static struct CRay{
    float rayAngle;
    Vector2 wallHit;
    float distance;
    bool isHitVertical;
    bool isRayFacingUp;
    bool isRayFacingDown; 
    bool isRayFacingRight; 
    bool isRayFacingLeft; 
    int wallHitContent;
} rays[NUM_RAYS];


void drawRaysMinimap(Vector3 playerWorldPosition, float playerAngle);
void raycastingInit();
void castAllRays(void);
#endif