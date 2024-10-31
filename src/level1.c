#include <math.h>
#include "raylib.h"
#include "level1.h"

extern Color palette[8];

void DrawLevel1Map(Vector3 playerWorldPosition, float playerRotationAngle)
{
    //the map is 32 x 32 tiles, to get the tile size, divide the shortest size by 32
    float tileSize = GetScreenHeight() / L1ROWS;
    float playerSize = tileSize;
    float horizontalOffset = (GetScreenWidth() - 32 * tileSize) / 2;

    for(int y=0; y<L1ROWS; y++)
    {
        for(int x=0; x<L1COLS; x++)
        {
            DrawRectangle(horizontalOffset + x*tileSize, y*tileSize, tileSize, tileSize, palette[Level1[0][y][x]]);

            float playerScreenX = horizontalOffset + playerWorldPosition.x * tileSize + tileSize / 2;
            float playerScreenY = playerWorldPosition.y * tileSize + tileSize / 2;
            // Draw the player
            DrawCircle(playerScreenX, playerScreenY, playerSize / 2, palette[7]);
            DrawLine(playerScreenX, playerScreenY, playerScreenX + (cosf(playerRotationAngle) * playerSize), playerScreenY + (sinf(playerRotationAngle) * playerSize), palette[7]);
        }
    }
}

Vector3 GetLevel1SpawnPoint(int layer)
{
    return Level1Start;
}