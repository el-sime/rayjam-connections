#include "raylib.h"
#include "constants.h"
#include "screens.h"

static bool isScreenFinished = false;

void UpdateDrawTitleScreen(void)
{
    if(IsKeyPressed(KEY_SPACE))
    {
        isScreenFinished = true;
    }
    BeginDrawing();
    ClearBackground(CBLUE);
    float titleWidth = MeasureText("Catch the connection", 60);
    DrawText("Catch the connection", GetScreenWidth() / 2 - titleWidth / 2, 40, 60, CYELLOW );
    float catchPhraseWidth = MeasureText("Based on a true story", 20);
    DrawText("Based on a true story", GetScreenWidth() / 2 - catchPhraseWidth / 2, 100, 20, CYELLOW );
    float pressSpaceWidth = MeasureText("Press [SPACE] to start", 30);
    // DrawText("Have you ever tried to catch a train connection in Paris ?\n This is how it feels like")
    DrawText("Press [SPACE] to start", GetScreenWidth() / 2 - pressSpaceWidth / 2, GetScreenHeight() - 128, 30, CORANGE );
    EndDrawing();

}
void InitTitleScreen(void)
{
    isScreenFinished = false;
}
bool IsTitleScreenFinished(void)
{
    return isScreenFinished;
}
