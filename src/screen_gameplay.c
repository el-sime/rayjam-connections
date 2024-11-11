#include <stdio.h>
#include "raylib.h"
#include "constants.h"
#include "screens.h"
#include "screen_gameplay.h"
#include "cubemap.h"
#include "player.h"




void InitGameplayScreen(void)
{

    camera.position = (Vector3){ 0.0f, 0.4f, 0.0f };
    camera.target = (Vector3){ 2.0f, 0.4f, 15.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    smallOuch = LoadSound("resources/sounds/smallouch.ogg");
    bigOuch   = LoadSound("resources/sounds/OUCH.ogg");
    OuhMother = LoadSound("resources/sounds/OUH_MOTHER.ogg");
    scratch = LoadSound("resources/sounds/scratch.mp3");
    tunnelVision = LoadMusicStream("resources/sounds/tunnel_vision.xm");
    //tunnelVision
    tunnelVision.looping = true;
    SetMusicVolume(tunnelVision, 0.3);
    InitPlayer(&player);

    state = STATE_PLAYING;
    atlasTexture = LoadTexture("resources/connections-atlas.png");    // Load map texture
    LoadLevel(1, &player);
    mapPosition.x = 0.0f;
    mapPosition.y = 0.0f;
    mapPosition.z = 0.0f;
}

static CollisionType CheckPlayerWallsCollision(Player *player, float deltaTime)
{
    // check player collisions 
    float playerNewX = player->worldPosition.x + cosf(player->rotationAngle) * (player->speed * deltaTime);
    float playerNewY = player->worldPosition.y + sinf(player->rotationAngle) * (player->speed * deltaTime);

    int pixelX = playerNewX > player->worldPosition.x ? floorf(playerNewX) : ceilf(playerNewX); 
    int pixelY = playerNewY > player->worldPosition.y ? floorf(playerNewY) : ceilf(playerNewY);

    CollisionType collision = NO_COLLISION;

    for (int y = 0; (y < mapTexture.height && collision == NO_COLLISION) ; y++)
    {
        for (int x = 0; (x < mapTexture.width  && collision == NO_COLLISION); x++)
        {
            Color pixelColor = mapPixels[y*mapTexture.width + x];
            if ( !COLOR_EQUAL(pixelColor, CPURPLE) &&       // Collision: white pixel, only check R channel
                (CheckCollisionCircleRec((Vector2){playerNewX, playerNewY}, player->radius,
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
    return collision;
}

void UnloadGameplayScreen(void)
{
    UnloadTexture(mapTexture);    // Unload cubicmap texture
    UnloadTexture(atlasTexture);
    UnloadTexture(levelMapTexture);
    UnloadModel(model);         // Unload map model
    UnloadSound(smallOuch);
    UnloadSound(bigOuch);
    UnloadSound(OuhMother);
    UnloadSound(scratch);
    UnloadMusicStream(tunnelVision);
}

void UpdateDrawGameplayScreen(void)
{
    float deltaTime = GetFrameTime();
    BeginDrawing();
    ClearBackground(CBLUE);
    if(state == STATE_PLAYING)
    {
        levelTime -= deltaTime;
        BeginMode3D(camera);
            DrawModel(model, mapPosition, 1.0f, WHITE);
        EndMode3D();
        UpdateDrawPlayer(&player, deltaTime);
        DrawText(TextFormat("TIME LEFT: %.2f", levelTime),10, 10, 40, CYELLOW );
        DrawText(TextFormat("Speed: %.2f", player.speed),10, 40, 20, CYELLOW );
    }
   
    
    EndDrawing();
}

static void LoadLevel(int levelId, Player *player)
{
    char filename[28];
    sprintf(filename, "resources/levels/level%d.txt", levelId);
    
    FILE *levelFile;
    if((levelFile = fopen(filename, "r")) == NULL)
    {
        // panic
        levelId = -1;
        return;
    }
    int id;
    float x;
    float y;
    float z;
    float angle;

    int line = 0;
    fscanf(levelFile, "%d", &id);
    line++;
    while(!feof(levelFile))
    {
        if(line == 1) fscanf(levelFile, "%f", &levelTime);
        if(line == 2) fscanf(levelFile, "%f", &x);
        if(line == 3) fscanf(levelFile, "%f", &y);
        if(line == 4) fscanf(levelFile, "%f", &z);
        if(line == 5) fscanf(levelFile, "%f", &angle);
        line++;
    }
    fclose(levelFile);
    
    player->worldPosition.x = x;
    player->worldPosition.y = y;
    player->worldPosition.z = z;
    player->rotationAngle = angle;



    char mapFilename[24];
    sprintf(mapFilename, "resources/level%dmap.png", levelId);
    Image image = LoadImage(mapFilename);
    mapTexture = LoadTextureFromImage(image);
    mapPixels = LoadImageColors(image);
    
    Mesh mesh = GenMeshCubicmapMulticolor(image, (Vector3){ 1.0f, 1.0f, 1.0f }, CPURPLE, CBLUE);
    model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = atlasTexture;

    UnloadImage(image);
    return;
}


static void UpdateCustomCamera(Player *player)
{
    camera.position.x = player->worldPosition.x;
    camera.position.z = player->worldPosition.y;

    camera.target.x = camera.position.x + (cosf(player->rotationAngle) * 50);
    camera.target.z = camera.position.z + (sinf(player->rotationAngle) * 50);
    //DrawLine(playerScreenPosition.x, playerScreenPosition.y, playerScreenPosition.x + (cosf(playerRotationAngle) * playerMinimapSize), playerScreenPosition.y + (sinf(playerRotationAngle) * playerMinimapSize), palette[7]);
}
