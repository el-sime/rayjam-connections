#include "raylib.h"
#include "constants.h"
#include "screens.h"

static float logoScreenTime = 0.0;
static const float MAX_LOGO_SCREEN_TIME = 3.0;
static bool isScreenFinished = false;
void UpdateDrawLogoScreen(void)
{
    logoScreenTime += GetFrameTime();
    if( IsKeyPressed(KEY_SPACE) ||
        IsGestureDetected(GESTURE_TAP) ||
        logoScreenTime >= MAX_LOGO_SCREEN_TIME)
    {
        isScreenFinished = true;
    }
    BeginDrawing();
    ClearBackground(CBLUE);
    int LogoSquareMaxSize = GetScreenHeight() - 80;
    Rectangle LogoSquare = {
        GetScreenWidth() / 2 - (LogoSquareMaxSize / 2),
        GetScreenHeight() / 2 - (LogoSquareMaxSize / 2),
        LogoSquareMaxSize,
        LogoSquareMaxSize
    };
    float backgroundRectangleHeight = LogoSquareMaxSize / 6;
    int line = 0;
    for(int i=6; i>=1; i--)
    {   
        float y = LogoSquare.y + line * backgroundRectangleHeight;
        DrawRectangle(LogoSquare.x, y, LogoSquareMaxSize, backgroundRectangleHeight, palette[i]);
        line++;
    }
    DrawRectangleLinesEx(LogoSquare, 8, CYELLOW);
    float raylibX = GetScreenWidth() / 2 - MeasureText("raylib", 60) / 2;
    DrawText("powered by", LogoSquare.x, LogoSquare.y - 10, 10, CYELLOW);
    DrawText("raylib", raylibX, GetScreenHeight() / 2 - 40, 60, CYELLOW);
    EndDrawing();
    return;
}

bool IsScreenLogoFinished(void)
{
    return isScreenFinished;
}