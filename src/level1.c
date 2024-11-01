#include <math.h>
#include "raylib.h"
#include "level1.h"

extern Color palette[8];

void DrawLevel1Map(Vector3 playerWorldPosition, float playerRotationAngle)
{
    //the map is 32 x 32 tiles, to get the tile size, divide the shortest size by 32
    float tileSize = GetScreenHeight() / L1ROWS;
    for(int y=0; y<L1ROWS; y++)
    {
        for(int x=0; x<L1COLS; x++)
        {
            Vector2 tileScreenPosition = GetTileScreenPosition(x, y, 0);
            DrawRectangle(tileScreenPosition.x, tileScreenPosition.y, tileSize, tileSize, palette[Level1[0][y][x]]);
            DrawRectangleLines(tileScreenPosition.x,  tileScreenPosition.y, tileSize, tileSize, BLACK);
            
        }
    }
}

Vector3 GetLevel1SpawnPoint(int layer)
{
    return Level1Start;
}

bool GetLevel1WallCollision(Vector3 playerWorldPosition, Vector3 playerNewPosition, float playerSize)
{
    float tileSize = GetScreenHeight() / L1ROWS;
    int x,y,z;
    z = 0;
    x = (playerWorldPosition.x < playerNewPosition.x) ? ceil(playerNewPosition.x) : floor(playerNewPosition.x);
    y = (playerWorldPosition.y < playerNewPosition.y) ? ceil(playerNewPosition.y) : floor(playerNewPosition.y);
    bool collision = !(Level1[z][y][x] == L1FLOOR_TILE);
    return collision;
}

static Vector2 GetTileScreenPosition(int x, int y, int z)
{
    float tileSize = GetScreenHeight() / L1ROWS;
    float horizontalOffset = (GetScreenWidth() - 32 * tileSize) / 2;
    Vector2 screenPosition;
    screenPosition.x = horizontalOffset + x * tileSize;
    screenPosition.y = y * tileSize;
    return screenPosition;
}

Vector3 GetPlayerLevel1ScreenPosition(Vector3 playerWorldPosition, float playerRotationAngle)
{
    //the map is 32 x 32 tiles, to get the tile size, divide the shortest size by 32
    float tileSize = GetScreenHeight() / L1ROWS;
    float playerSize = tileSize;
    float horizontalOffset = (GetScreenWidth() - 32 * tileSize) / 2;
    float playerScreenX = horizontalOffset + playerWorldPosition.x * tileSize + tileSize / 2;
    float playerScreenY = playerWorldPosition.y * tileSize + tileSize / 2;
        // Draw the player
        //DrawCircle(playerScreenX, playerScreenY, playerSize / 2, palette[7]);
        //DrawLine(playerScreenX, playerScreenY, playerScreenX + (cosf(playerRotationAngle) * playerSize), playerScreenY + (sinf(playerRotationAngle) * playerSize), palette[7]);
    return (Vector3){playerScreenX, playerScreenY, 0};
}