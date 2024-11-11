/*******************************************************************************************
*
*   raylib gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 5.0
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "constants.h"
#include "screens.h"
#include "cubemap.h"

#define MAX_LEVEL 2

Color palette[MAX_PALETTE_COLORS] = {
    CBLUE    ,
    CPURPLE  ,
    CVIOLET  ,
    CBURGUNDY,
    CDARKRED ,
    CRED     ,
    CORANGE  ,
    CYELLOW
};


#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for: 

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef struct Level {
    int id;
    float time;
    Vector3 startingPosition;
} Level;

Level LoadLevel(int levelId);
void UnloadLevel(void);


//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
GameScreen currentGameScreen;

static const int screenWidth = 800;
static const int screenHeight = 450;


static float logoScreenTime = 0.0;
static float logoScreenMaxTime = 3.0;
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Catch the connection");
    currentGameScreen = SCREEN_LOGO;
    // TODO: Load resources / Initialize variables at this point
    // Define the camera to look into our 3d world
    
    InitAudioDevice();
    // TODO: refactor by screen
    
    
    DisableCursor();  

    


#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);     // Set our game frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------

    

    
    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    if(currentGameScreen == SCREEN_LOGO)
    {
        UpdateDrawLogoScreen();
        if (IsLogoScreenFinished()) currentGameScreen = SCREEN_TITLE;
        return;
    }
    if(currentGameScreen == SCREEN_TITLE)
    {
        UpdateDrawTitleScreen();
        if (IsTitleScreenFinished()) currentGameScreen = SCREEN_GAMEPLAY;
        return;
    }

   
    if(isGameOver > -1){
        UpdateDrawGameOver();
        return;
    }
    if (currentLevel == 0)
    {
        LoadLevel(++currentLevel);
        return;
    }
    
    UpdateMusicStream(tunnelVision);
    // Update
    //----------------------------------------------------------------------------------
    

    levelTime -= GetFrameTime();

    if(levelTime <= 0.0f)
    {
        isGameOver = TIMESUP;
    }

    MovePlayer();
    UpdateCustomCamera();

    BeginDrawing();
        ClearBackground(CBLUE);
        BeginMode3D(camera);

            DrawModel(model, mapPosition, 1.0f, WHITE);

        EndMode3D();
        

        
        
       
        

        DrawText(TextFormat("TIME LEFT: %.2f", levelTime),10, 10, 40, CYELLOW );
        DrawText(TextFormat("Speed: %.2f", playerSpeed),10, 40, 20, CYELLOW );
        DrawRectangle(GetScreenWidth()/2 - 52, 4, 104, 44, CPURPLE);
        DrawRectangle(GetScreenWidth()/2 - 50, 6, playerLife, 40, CORANGE);
    EndDrawing();
    //----------------------------------------------------------------------------------  
}



static void MovePlayer(void)
{
    if( collision != NO_COLLISION)
    {
        if(collision == WALL || collision == OBSTACLE)
        {
            if(playerGraceTime <= 0)
            {
                //calculate damage based on speed
                float dmg = playerSpeed * 10.0f;
                playerLife -= dmg;
                if(dmg <= 15.0f)
                {
                    PlaySound(smallOuch);
                }
                if(dmg > 15.0f && dmg <= 35.0f)
                {
                    PlaySound(bigOuch);
                }
                if(dmg > 35.0f)
                {
                    PlaySound(OuhMother);
                }
                if (playerLife <= 0)
                {
                    isGameOver = DEAD;
                }
                playerGraceTime = playerBaseGraceTime;
            }
        }
        else if(collision == BADTRAIN)
        {
            isGameOver = WRONGTRAIN;
        }
        else if(collision == EXIT)
        {
            isGameOver = LEFTSTATION;
        }
        else if(collision == CONNECTION)
        {
            isGameOver = WON;
        }
    }
    else
    {
        playerWorldPosition.x = playerNewX;
        playerWorldPosition.y = playerNewY;
    }
}

static void UpdateCustomCamera(void)
{
    camera.position.x = playerWorldPosition.x;
    camera.position.z = playerWorldPosition.y;

    camera.target.x = camera.position.x + (cosf(playerRotationAngle) * 50);
    camera.target.z = camera.position.z + (sinf(playerRotationAngle) * 50);
    //DrawLine(playerScreenPosition.x, playerScreenPosition.y, playerScreenPosition.x + (cosf(playerRotationAngle) * playerMinimapSize), playerScreenPosition.y + (sinf(playerRotationAngle) * playerMinimapSize), palette[7]);
}

static void UpdateDrawGameOver(void)
{
    if(IsKeyPressed(KEY_SPACE))
    {
        if(isGameOver == WON && currentLevel + 1 <= MAX_LEVEL)
        {
            LoadLevel(++currentLevel);
            return;
        }
        else
        {
            currentGameScreen = SCREEN_TITLE;
            isGameOver = NOT_OVER;
            didScratchPlay = false;
            currentLevel = 0;
        }
    }
    int fontSize = 48;
    BeginDrawing();
    if(isGameOver == WON)
    {
        ClearBackground(CPURPLE);
        const char *gameOverText = "YOU WON!";
        DrawText(gameOverText, GetScreenWidth()/2 - MeasureText(gameOverText, fontSize) / 2, GetScreenHeight()/2 - fontSize / 2, fontSize, CYELLOW );
    }
    else
    {
        if(!didScratchPlay)
        {
            PlaySound(scratch);
            didScratchPlay = true;
        }
        if(IsMusicStreamPlaying(tunnelVision))
        {
            StopMusicStream(tunnelVision);
        }
        ClearBackground(CDARKRED);
        const char *gameOverLabel = "GAME OVER";
        DrawText(gameOverLabel, GetScreenWidth()/2 - MeasureText(gameOverLabel, fontSize) / 2, GetScreenHeight()/4 - fontSize / 2, fontSize, CORANGE );
        if(isGameOver == DEAD)
        {
            const char *gameOverText = "YOU DIED";
            DrawText(gameOverText, GetScreenWidth()/2 - MeasureText(gameOverText, fontSize) / 2, GetScreenHeight()/2 - fontSize / 2, fontSize, CYELLOW );
        }
        else if(isGameOver == TIMESUP)
        {
            const char *gameOverText = "YOU MISSED YOUR CONNECTION";
            DrawText(gameOverText, GetScreenWidth()/2 - MeasureText(gameOverText, fontSize) / 2, GetScreenHeight()/2 - fontSize / 2, fontSize, CYELLOW );
        }
        else if(isGameOver == WRONGTRAIN)
        {
            const char *gameOverText = "YOU GOT ON THE WRONG TRAIN";
            DrawText(gameOverText, GetScreenWidth()/2 - MeasureText(gameOverText, fontSize) / 2, GetScreenHeight()/2 - fontSize / 2, fontSize, CYELLOW );
        }
        else if(isGameOver == LEFTSTATION)
        {
            const char *gameOverText = "YOU LEFT THE STATION";
            DrawText(gameOverText, GetScreenWidth()/2 - MeasureText(gameOverText, fontSize) / 2, GetScreenHeight()/2 - fontSize / 2, fontSize, CYELLOW );
        }
    }       
    float pressSpaceWidth = MeasureText("Press [SPACE] to start", 30);
    DrawText("Press [SPACE] to start", GetScreenWidth() / 2 - pressSpaceWidth / 2, GetScreenHeight() - 128, 30, CORANGE );
    EndDrawing();
}






void UnloadLevel(void)
{
    return;
}
Level LoadLevel(int levelId)
{
    Level lev;
    int id;
    float time;
    float x;
    float y;
    float z;
    float startingAngle;

    isGameOver = NOT_OVER;
    playerLife = 100.0f;
    playerSpeed = 0.0f;
    playerRotationAngle =  PI * 1.5;
    playerTurnDirection = 0;
    char filename[28];
    sprintf(filename, "resources/levels/level%d.txt", levelId);
    FILE *levelFile;
    if((levelFile = fopen(filename, "r")) == NULL)
    {
        // panic
        lev.id = -1;
        return lev;
    }

    int line = 0;
    fscanf(levelFile, "%d", &id);
    line++;
    while(!feof(levelFile))
    {
        if(line == 1) fscanf(levelFile, "%f", &time);
        if(line == 2) fscanf(levelFile, "%f", &x);
        if(line == 3) fscanf(levelFile, "%f", &y);
        if(line == 4) fscanf(levelFile, "%f", &z);
        if(line == 5) fscanf(levelFile, "%f", &startingAngle);
        line++;
    }
    fclose(levelFile);
    lev.id = id;
    lev.time = time;
    lev.startingPosition.x = x;
    lev.startingPosition.y = y;
    lev.startingPosition.z = z;

    levelTime = time;
    char mapFilename[24];
    sprintf(mapFilename, "resources/level%dmap.png", levelId);
    Image image = LoadImage(mapFilename);
    mapTexture = LoadTextureFromImage(image);

    char pauseMapFilename[30];
    sprintf(pauseMapFilename, "resources/level%dmap_pause.png", levelId);
    levelMapTexture = LoadTexture(pauseMapFilename);

    Mesh mesh = GenMeshCubicmapMulticolor(image, (Vector3){ 1.0f, 1.0f, 1.0f }, CPURPLE, CBLUE);
    model = LoadModelFromMesh(mesh);

    atlasTexture = LoadTexture("resources/connections-atlas.png");    // Load map texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = atlasTexture;    // Set map diffuse texture

    // Get map image data to be used for collision detection
    mapPixels = LoadImageColors(image);

    playerWorldPosition.x = lev.startingPosition.x;
    playerWorldPosition.y = lev.startingPosition.y;
    playerWorldPosition.z = lev.startingPosition.z;

    UnloadImage(image);     // Unload cubesmap image from RAM, already uploaded to VRAM
    PlayMusicStream(tunnelVision);
    return lev;
}