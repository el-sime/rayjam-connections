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

#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif



//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
GameScreen currentGameScreen;

static const int screenWidth = 800;
static const int screenHeight = 450;
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
    InitWindow(screenWidth, screenHeight, "Catch the connection");
    currentGameScreen = SCREEN_LOGO;
    InitAudioDevice();    
    //DisableCursor();  
    //SetExitKey(KEY_NULL);       // Disable KEY_ESCAPE to close window, X-button still works

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

    CloseWindow();
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
        if (IsTitleScreenFinished())
        {
            currentGameScreen = SCREEN_GAMEPLAY;
            InitGameplayScreen();

        }
        return;
    }
    if(currentGameScreen == SCREEN_GAMEPLAY)
    {
        UpdateDrawGameplayScreen();
        if(IsGameplayScreenFinished())
        {
            currentGameScreen = SCREEN_TITLE;
            UnloadGameplayScreen();
            InitTitleScreen();
        }
        return;
    }
}