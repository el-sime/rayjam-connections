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
static const int screenWidth = 800;
static const int screenHeight = 450;

static const int minimapSize = 100;

static Vector2 playerMapPosition;



Camera camera = { 0 };
Vector3 mapPosition = { 0.0f, 0.0f, 0.0f };          // Set model position
Model model;
Texture2D mapTexture;

// Player
static Vector3 playerWorldPosition = {0.0f, 0.0f, 0.0f};
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

Texture2D atlasTexture;
static Color *mapPixels; // for collisions

static Sound smallOuch;
static Sound bigOuch  ;
static Sound OuhMother;
static Sound scratch;

static Music tunnelVision;

static GameOverReason isGameOver = NOT_OVER;
static bool didScratchPlay = false;


static bool isGamePaused = true;

static int currentLevel = 0;
static Texture2D levelMapTexture;
static float levelTime = 0.0f;


GameScreen currentGamescreen = SCREEN_LOGO;


static float logoScreenTime = 0.0;
static float logoScreenMaxTime = 3.0;
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
static void MovePlayer(void);
static void UpdateCustomCamera(void);
static void UpdateDrawGameOver(void);

static void UpdateDrawLogoScreen(void);
static void UpdateDrawTitleScreen(void);
static void UpdateDrawPauseScreen(void);
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
    
    // TODO: Load resources / Initialize variables at this point
    // Define the camera to look into our 3d world
    
    camera.position = (Vector3){ 0.0f, 0.4f, 0.0f };
    camera.target = (Vector3){ 2.0f, 0.4f, 15.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    
    
    InitAudioDevice();
    // TODO: refactor by screen
    smallOuch = LoadSound("resources/sounds/smallouch.ogg");
    bigOuch   = LoadSound("resources/sounds/OUCH.ogg");
    OuhMother = LoadSound("resources/sounds/OUH_MOTHER.ogg");
    scratch = LoadSound("resources/sounds/scratch.mp3");
    tunnelVision = LoadMusicStream("resources/sounds/tunnel_vision.xm");
    //tunnelVision
    tunnelVision.looping = true;
    SetMusicVolume(tunnelVision, 0.3);
    

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
    UnloadTexture(atlasTexture);
    UnloadTexture(levelMapTexture);
    UnloadModel(model);         // Unload map model
    UnloadSound(smallOuch);
    UnloadSound(bigOuch);
    UnloadSound(OuhMother);
    UnloadSound(scratch);
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
    if(currentGamescreen == SCREEN_LOGO)
    {
        UpdateDrawLogoScreen();
        return;
    }
    if(currentGamescreen == SCREEN_TITLE)
    {
        UpdateDrawTitleScreen();
        return;
    }

    // if(isGamePaused)
    // {
    //     UpdateDrawPauseScreen();
    // }

    if(playerSpeed == 0) playerAnimationFrameDuration = 100000;//horrible hack
    if(playerSpeed >= 1 && playerSpeed < 2.5) playerAnimationFrameDuration = 0.3;
    if(playerSpeed >= 2.5 && playerSpeed < 3.5) playerAnimationFrameDuration = 0.2;
    if(playerSpeed >= 3.5 && playerSpeed <= 5) playerAnimationFrameDuration = 0.1;

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

        DrawText(TextFormat("TIME LEFT: %.2f", levelTime),10, 10, 40, CYELLOW );
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
                if(COLOR_EQUAL(pixelColor, BLACK))  collision = EXIT;
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
    if(IsKeyPressed(KEY_SPACE))
    {
        if(isGameOver == WON && currentLevel + 1 <= MAX_LEVEL)
        {
            LoadLevel(++currentLevel);
            return;
        }
        else
        {
            currentGamescreen = SCREEN_TITLE;
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



static void UpdateDrawLogoScreen(void)
{
    logoScreenTime += GetFrameTime();
    if(logoScreenTime >= logoScreenMaxTime)
    {
        //currentGamescreen = SCREEN_TITLE;
    }
    if(IsKeyPressed(KEY_SPACE))
    {
        currentGamescreen = SCREEN_TITLE;
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
}

static void UpdateDrawTitleScreen(void)
{
    if(IsKeyPressed(KEY_SPACE))
    {
        currentGamescreen = SCREEN_GAMEPLAY;
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
static void UpdateDrawPauseScreen(void)
{
    return;
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