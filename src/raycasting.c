#include <math.h>
#include "constants.h"
#include "raylib.h"
#include "raycasting.h"
#include "level1.h"

extern Color palette[8];

void castAllRays(Vector3 playerWorldPosition, float playerAngle)
{
    float rayAngle = playerAngle - (FOV_ANGLE / 2);
    for (int r = 0; r < NUM_RAYS; r++)
    {
        
        rayAngle += FOV_ANGLE / NUM_RAYS; 
    }
}
//TODO: create a Ray factory from player position and angle
//TODO: Create a map manager 
void castRay(float rayAngle, int rayIndex, Vector3 playerWorldPosition)
{
    /****************************/
    /*       Ray factory        */
    /****************************/
    float xIntersect, yIntersect;
    float xIncrement, yIncrement;
    // 1 - normalize the angle
    rayAngle = normalizeRayAngle(rayAngle);

    // 2 - determine direction
    bool isRayFacingUp = rayAngle > 0 && rayAngle < PI;
    bool isRayFacingDown = !isRayFacingUp;
    bool isRayFacingRight = rayAngle < PI * 0.5 || rayAngle > PI * 1.5;
    bool isRayFacingLeft = !isRayFacingRight;

    // 3 - find the first horizontal intersection 
    bool isHorizontalIntersectionFound = false;
    float horizontalIntersectionX = 0.0f;
    float horizontalIntersectionY = 0.0f;
    int wallTileId = -1;

    // 3.1 - find the Y coordinate of the closest horizontal grid intersection
    horizontalIntersectionY = floorf(playerWorldPosition.y / TILE_SIZE) * TILE_SIZE;
    horizontalIntersectionY += isRayFacingDown ? TILE_SIZE : 0;

    // 3.2 - find the X coordinate of the closest horizontal grid intersection
    horizontalIntersectionX = playerWorldPosition.x + (horizontalIntersectionY - playerWorldPosition.y) / tanf(rayAngle);

    // 4 - calculate increments
    yIncrement = TILE_SIZE;
    yIncrement *= isRayFacingUp ? -1 : 1;

    xIncrement = TILE_SIZE / tanf(rayAngle);
    xIncrement *= (isRayFacingLeft && xIncrement > 0) ? -1 : 1;
    xIncrement *= (isRayFacingRight && xIncrement < 0) ? -1 : 1;

    // 5 - find the wall
    float nextHorizontalX;
    float nextHorizontalY;
    while(
        nextHorizontalX >= 0 && nextHorizontalX <= GetScreenWidth() &&
        nextHorizontalY >= 0 && nextHorizontalY <= GetScreenHeight() 
    )
    {
        float tileOriginX = roundf(nextHorizontalX) + (false) ? 1 : 0;
        float tileOriginY = roundf(nextHorizontalY) + (isRayFacingUp) ? -1 : 0;
        wallTileId = GetLevel1TileContent(tileOriginX, tileOriginY, 0);
        if (wallTileId != L1FLOOR_TILE) {
            horizontalIntersectionX = nextHorizontalX;
            horizontalIntersectionY = nextHorizontalY;
            isHorizontalIntersectionFound = true;
            break;
        }else{
            nextHorizontalX += xIncrement;
            nextHorizontalY += yIncrement;
        }
    }

    return;
}

static float normalizeRayAngle(float rayAngle)
{
    float normalizedAngle = 0.0f;
    rayAngle = remainderf(rayAngle, (2 * PI));
    normalizedAngle = rayAngle < 0 ? (2 * PI) + rayAngle : rayAngle;
    return normalizedAngle;
}