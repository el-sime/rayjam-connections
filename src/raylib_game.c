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
#include <math.h>
#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for: 

// Levels
#include "level1.h"



//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------

/**********************************/
//        8 COLORS PALETTE        //
/**********************************/

#define CBLUE     CLITERAL (Color){0x00, 0x3f, 0x5c, 0xff}
#define CPURPLE   CLITERAL (Color){0x58, 0x50, 0x8d, 0xff}
#define CVIOLET   CLITERAL (Color){0x8a, 0x50, 0x8f, 0xff}
#define CBURGUNDY CLITERAL (Color){0xbc, 0x50, 0x90, 0xff}
#define CDARKRED  CLITERAL (Color){0xde, 0x5a, 0x79, 0xff}
#define CRED      CLITERAL (Color){0xff, 0x63, 0x31, 0xff}
#define CORANGE   CLITERAL (Color){0xff, 0x85, 0x31, 0xff}
#define CYELLOW   CLITERAL (Color){0xff, 0xa6, 0x00, 0xff}

Color palette[8] = {
    CBLUE    ,
    CPURPLE  ,
    CVIOLET  ,
    CBURGUNDY,
    CDARKRED ,
    CRED     ,
    CORANGE  ,
    CYELLOW
};


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
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;

// TODO: Define your custom data types here

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static RenderTexture2D target = { 0 };  // Render texture to render our game

// TODO: Define global variables here, recommended to make them static

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
static void MovePlayer(void);
//static void DrawPlayer(void);


static float minimapScale = 0.25f;
static int currentLevel = 1;

static Vector3 playerWorldPosition;
static float playerMinSpeed = 1.0f; // pixels /s 
static float playerMaxSpeed = 5.0f; // pixels /s 
static float playerSpeed; // pixels / sec
static float playerAcceleration = 10;
static float playerRotationAngle =  PI * 1.5;//PI * 1.5;
static int playerTurnDirection = 0;
static float playerTurnSpeed = 90 * (PI / 180); // 90 deg per second

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
    InitWindow(screenWidth, screenHeight, "Metro Connections");
    
    // TODO: Load resources / Initialize variables at this point
    playerWorldPosition = GetLevel1SpawnPoint();
    playerSpeed = 0.0f;

    // Render texture to draw full screen, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

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
    UnloadRenderTexture(target);
    
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
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------
    if(IsKeyPressed(KEY_A))
    {
        playerTurnDirection = -1;
    }
    if(IsKeyPressed(KEY_D))
    {
        playerTurnDirection = 1;
    }
    if(IsKeyPressed(KEY_SPACE))
    {
        playerSpeed = playerMinSpeed;
    }
    if(
        (playerTurnDirection == -1 && IsKeyReleased(KEY_A)) ||
        (playerTurnDirection == 1 && IsKeyReleased(KEY_D))
    ) 
    {
        playerTurnDirection = 0;
    }

    if(IsKeyDown(KEY_W))
    {
        playerSpeed = Clamp(playerSpeed + playerAcceleration * GetFrameTime(), playerMinSpeed, playerMaxSpeed);
    }
    if(IsKeyDown(KEY_S))
    {
        playerSpeed = Clamp(playerSpeed - playerAcceleration * GetFrameTime(), playerMinSpeed, playerMaxSpeed);
    }

    MovePlayer();
    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture, 
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        DrawLevel1Map(playerWorldPosition, playerRotationAngle);
        
    EndTextureMode();
    
    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(CBLUE);
        
        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawText(TextFormat("Speed: %f", playerSpeed), 20, 20, 20, CYELLOW);
        // TODO: Draw everything that requires to be drawn at this point, maybe UI?

    EndDrawing();
    //----------------------------------------------------------------------------------  
}

static void MovePlayer(void)
{
    playerRotationAngle += playerTurnDirection * playerTurnSpeed * GetFrameTime();
    float playerNewX = playerWorldPosition.x + cosf(playerRotationAngle) * (playerSpeed * GetFrameTime());
    float playerNewY = playerWorldPosition.y + sinf(playerRotationAngle) * (playerSpeed * GetFrameTime());
    //TODO: check collisions

    playerWorldPosition.x = playerNewX;
    playerWorldPosition.y = playerNewY;
}