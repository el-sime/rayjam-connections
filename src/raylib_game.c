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
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;

// TODO: Define your custom data types here
typedef enum {
    NOT_OVER = -1,
    DEAD = 0,
    TIMESUP,
    WRONGTRAIN,
    LEFTSTATION,
    WON
} GameOverReason;

typedef enum {
    NO_COLLISION = -1,
    WALL = 0,
    BADTRAIN,
    EXIT,
    OBSTACLE,
    CONNECTION
} CollisionType;
//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static const int minimapSize = 100;

static Vector2 playerMapPosition;



Camera camera = { 0 };
Vector3 mapPosition = { 0.0f, 0.0f, 0.0f };          // Set model position
Model model;
Texture2D mapTexture;

// Player
static Vector3 playerWorldPosition = {34.0f, 89.0f, 0.0f};
static float playerMinSpeed = 1.0f; // pixels /s 
static float playerMaxSpeed = 5.0f; // pixels /s 
static float playerSpeed; // pixels / sec
static float playerAcceleration = 10;
static float playerRotationAngle =  PI * 1.5;//PI * 1.5;
static int playerTurnDirection = 0;
static float playerTurnSpeed = 90 * (PI / 180); // 90 deg per second
static float playerLife = 100.0f;
static float playerBaseGraceTime = 1.5f;
static float playerGraceTime = 0;
static float playerMinimapSize = 8;
static bool isPlayerVisible = true;
static int playerAnimationFrame = 0;
static float playerAnimationTime = 0.0f;
static int playerAnimationMaxFrames = 5;
static float playerAnimationFrameDuration = 0.3;// = 100 / 5;
static int playerAnimationFrameWidth;
static Texture2D playerAnimationTexture;

static Color *mapPixels;

static Sound smallOuch;
static Sound bigOuch  ;
static Sound OuhMother;

static Music tunnelVision;

static GameOverReason isGameOver = NOT_OVER;
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
static void MovePlayer(void);
static void UpdateCustomCamera(void);
static void UpdateDrawGameOver(void);
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
    InitWindow(screenWidth, screenHeight, "Train Connections");
    
    // TODO: Load resources / Initialize variables at this point
    // Define the camera to look into our 3d world
    
    camera.position = (Vector3){ 34.0f, 0.4f, 89.0f };     // Camera position
    camera.target = (Vector3){ 2.0f, 0.4f, 15.0f };        // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                 // Camera projection type

    Image image = LoadImage("resources/level2map.png");      // Load cubicmap image (RAM)
    mapTexture = LoadTextureFromImage(image);

    Mesh mesh = GenMeshCubicmapMulticolor(image, (Vector3){ 1.0f, 1.0f, 1.0f }, CPURPLE, CBLUE);
    model = LoadModelFromMesh(mesh);

    Texture2D texture = LoadTexture("resources/connections-atlas.png");    // Load map texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;    // Set map diffuse texture

    // Get map image data to be used for collision detection
    mapPixels = LoadImageColors(image);
    UnloadImage(image);     // Unload cubesmap image from RAM, already uploaded to VRAM
    
    InitAudioDevice();
    // TODO: refactor by screen
    smallOuch = LoadSound("resources/sounds/smallouch.ogg");
    bigOuch   = LoadSound("resources/sounds/OUCH.ogg");
    OuhMother = LoadSound("resources/sounds/OUH_MOTHER.ogg");
    tunnelVision = LoadMusicStream("resources/sounds/tunnel_vision.xm");
    //tunnelVision
    tunnelVision.looping = true;
    SetMusicVolume(tunnelVision, 0.5);
    PlayMusicStream(tunnelVision);

    playerAnimationTexture = LoadTexture("resources/player_spritesheet.png");
    playerAnimationFrameWidth = playerAnimationTexture.width / playerAnimationMaxFrames;
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
    UnloadTexture(mapTexture);    // Unload cubicmap texture
    UnloadTexture(texture);     // Unload map texture
    UnloadModel(model);         // Unload map model
    UnloadSound(smallOuch);
    UnloadSound(bigOuch);
    UnloadSound(OuhMother);
    UnloadMusicStream(tunnelVision);
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
    if(playerSpeed == 0) playerAnimationFrameDuration = 100000;
    if(playerSpeed >= 1 && playerSpeed < 2.5) playerAnimationFrameDuration = 0.3;
    if(playerSpeed >= 2.5 && playerSpeed < 3.5) playerAnimationFrameDuration = 0.2;
    if(playerSpeed >= 3.5 && playerSpeed <= 5) playerAnimationFrameDuration = 0.1;

    UpdateMusicStream(tunnelVision);
    if(isGameOver > -1){
        UpdateDrawGameOver();
        return;
    }
    // Update
    //----------------------------------------------------------------------------------
    if(playerGraceTime > 0)
    {
        playerGraceTime -= GetFrameTime();
    }

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
        if (playerSpeed > 0) 
        {
            playerSpeed = 0;
        }
        else
        {
            playerSpeed = playerMinSpeed;
        }
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
    UpdateCustomCamera();

    BeginDrawing();
        ClearBackground(CBLUE);
        BeginMode3D(camera);

            DrawModel(model, mapPosition, 1.0f, WHITE);

        EndMode3D();
        //Draw minimap
        //DrawTextureEx(mapTexture, (Vector2){ GetScreenWidth() - mapTexture.width - 20, 20.0f }, 0.0f, 1.0f, WHITE);
        //DrawTexturePro()
        //DrawTextureEx(mapTexture, (Vector2){ GetScreenWidth() - mapTexture.width*4.0f - 20, 20.0f }, 0.0f, 4.0f, WHITE);
        //DrawRectangle(GetScreenWidth() - mapTexture.width*4 - 20 + playerCellX*4, 20 + playerCellY*4, 4, 4, RED);
        
        
        // DrawRectangle(GetScreenWidth() - (minimapSize + 4), GetScreenHeight() - (minimapSize + 4), minimapSize + 4, minimapSize + 4, CYELLOW);
        // DrawRectangle(GetScreenWidth() - (minimapSize + 2), GetScreenHeight() - (minimapSize + 2), minimapSize, minimapSize, CBLUE);
        
        DrawTexture (mapTexture, GetScreenWidth() - mapTexture.width, GetScreenHeight() - mapTexture.height, WHITE);
        DrawRectangleLines((GetScreenWidth() - (minimapSize / 2) - 2) - minimapSize / 4, (GetScreenHeight() - (minimapSize / 2) - 2) - minimapSize / 4, minimapSize / 2, minimapSize / 2, CYELLOW);
        // DrawTexturePro(
        //     mapTexture,
        //     (Rectangle){playerWorldPosition.x - minimapSize / 4, playerWorldPosition.y - minimapSize / 4, minimapSize / 2, minimapSize / 2},
        //     (Rectangle){GetScreenWidth() - (minimapSize + 2), GetScreenHeight() - (minimapSize + 2), minimapSize, minimapSize},
        //     (Vector2){0,0},
        //     (2* PI ) - playerRotationAngle,
        //     WHITE
        // );
        DrawCircle(GetScreenWidth() - (minimapSize / 2) - 2, GetScreenHeight() - (minimapSize / 2) - 2, 2, CRED);

        DrawFPS(10, 10);
        
        //Draw the player
        playerAnimationTime += GetFrameTime();
        if (playerAnimationTime >= playerAnimationFrameDuration)
        {
            playerAnimationTime = 0.0f;
            if(playerAnimationFrame < playerAnimationMaxFrames - 1)
            {
                playerAnimationFrame++;
            }
            else
            {
                playerAnimationFrame = 0;
            }
        }
        Rectangle sourceRec =      {playerAnimationFrame * playerAnimationFrameWidth, 0, playerAnimationFrameWidth, playerAnimationTexture.height};
        Rectangle destinationRec = {GetScreenWidth() / 2 - playerAnimationFrameWidth, GetScreenHeight() - playerAnimationTexture.height * 3, playerAnimationFrameWidth * 3, playerAnimationTexture.height * 3};

        DrawTexturePro(
            playerAnimationTexture,
            sourceRec,
            destinationRec,
            (Vector2){0,0},
            0.0f,
            WHITE
        );

        DrawText(TextFormat("Speed: %.2f", playerSpeed),10, 40, 20, CYELLOW );
        DrawRectangle(GetScreenWidth()/2 - 52, 4, 104, 44, CPURPLE);
        DrawRectangle(GetScreenWidth()/2 - 50, 6, playerLife, 40, CORANGE);
    EndDrawing();
    //----------------------------------------------------------------------------------  
}



static void MovePlayer(void)
{
    playerRotationAngle += playerTurnDirection * playerTurnSpeed * GetFrameTime();
    float playerNewX = playerWorldPosition.x + cosf(playerRotationAngle) * (playerSpeed * GetFrameTime());
    float playerNewY = playerWorldPosition.y + sinf(playerRotationAngle) * (playerSpeed * GetFrameTime());
    float playerRadius = 0.1f;
    //check next pixel for collisions 
    int pixelX = playerNewX > playerWorldPosition.x ? floorf(playerNewX) : ceilf(playerNewX); 
    int pixelY = playerNewY > playerWorldPosition.y ? floorf(playerNewY) : ceilf(playerNewY);
    Color pixelColor = mapPixels[pixelY * mapTexture.width + pixelX];
    CollisionType collision = NO_COLLISION;
    for (int y = 0; (y < mapTexture.height && collision == NO_COLLISION) ; y++)
    {
        for (int x = 0; (x < mapTexture.width  && collision == NO_COLLISION); x++)
        {
            Color pixelColor = mapPixels[y*mapTexture.width + x];
            if ( !COLOR_EQUAL(pixelColor, CPURPLE) &&       // Collision: white pixel, only check R channel
                (CheckCollisionCircleRec((Vector2){playerNewX, playerNewY}, playerRadius,
                (Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
            {
                collision = WALL;
                //what kind of collision
                if(COLOR_EQUAL(pixelColor, CDARKRED)) collision = CONNECTION;
                if(COLOR_EQUAL(pixelColor, CORANGE))  collision = BADTRAIN;
                if(COLOR_EQUAL(pixelColor, CRED))  collision = EXIT;
            }
        }
    }

    
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
        ClearBackground(CDARKRED);
        
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
    
    EndDrawing();
}