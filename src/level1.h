#ifndef LEVEL1_H
#define LEVEL1_H
// for Vector3
#include "raylib.h"


#define L1COLS 32
#define L1ROWS 32
#define L1LAYERS 1

#define L1FLOOR_TILE 1

static const int Level1[L1LAYERS][L1ROWS][L1COLS] = {
    {
    //                       1                   2                   3
    //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//00
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//01
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//02
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//03
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//04
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//05
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//06
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//07
        {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},//08
        {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},//09
        {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},//10
        {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//11
        {0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//12
        {0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//13
        {0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//14
        {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//15
        {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//16
        {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//17
        {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},//18
        {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0},//19
        {5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0},//20
        {5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,3},//21
        {5,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,3},//22
        {0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,3},//23
        {0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},//24
        {0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},//25
        {0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},//26
        {0,0,0,2,1,1,1,2,0,2,1,1,1,2,0,2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0},//27
        {0,0,0,2,1,1,1,2,0,2,1,1,1,2,0,2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0},//28
        {0,0,0,2,1,1,1,2,0,2,1,1,1,2,0,2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0},//29
        {0,0,0,2,1,1,1,2,0,2,1,1,1,2,0,2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0},//30
        {0,0,0,2,1,1,1,2,0,2,5,5,5,2,0,2,5,5,5,2,0,0,0,0,0,0,0,0,0,0,0,0} //31
    //                       1                   2                   3
    //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    }
};

static Vector3 Level1Start = {5, 31, 0};
static Vector3 Level1End = {31, 22, 0};
static Vector2 GetTileScreenPosition(int x, int y, int z);


void DrawLevel1Map(Vector3 playerWorldPosition, float playerRotationAngle);

Vector3 GetLevel1SpawnPoint(int layer);

bool GetLevel1WallCollision(Vector3 playerWorldPosition, Vector3 playerNewPosition, float playerSize);

Vector3 GetPlayerLevel1ScreenPosition(Vector3 playerWorldPosition, float playerRotationAngle);

int GetLevel1TileContent(int x, int y, int z)
{
    if (x < 0 || x >=  L1COLS) return -1;
    if (y < 0 || y >=  L1ROWS) return -1;
    if(z != 0) return -1;
    return Level1[z][y][x];
}

#endif