#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "constants.h"
#include "screens.h"
#include "screen_gameplay.h"
#include "cubemap.h"
#include "player.h"
#include "actions.h"

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
    SetMusicVolume(tunnelVision, 0.1);
    InitPlayer(&player);

    heartTexture = LoadTexture("resources/heart.png");
    arrowTexture = LoadTexture("resources/arrow.png");
    arrowUpTexture = LoadTexture("resources/arrow-up.png");
    SetControlRectangle(LEFT, (Rectangle){0.0f,GetScreenHeight() - arrowTexture.height,arrowTexture.width, arrowTexture.height});
    SetControlRectangle(UP, (Rectangle){0.0f,GetScreenHeight() - arrowTexture.height*2,arrowTexture.width, arrowTexture.height});
    SetControlRectangle(RIGHT, (Rectangle){GetScreenWidth() - arrowTexture.width,GetScreenHeight() - arrowTexture.height,arrowTexture.width, arrowTexture.height});
    SetControlRectangle(DOWN, (Rectangle){GetScreenWidth() - arrowTexture.width,GetScreenHeight() - arrowTexture.height*2,arrowTexture.width, arrowTexture.height});

    atlasTexture = LoadTexture("resources/connections-atlas.png");    // Load map texture
    currentLevel = 1;
    LoadLevel(currentLevel, &player);
    mapPosition.x = 0.0f;
    mapPosition.y = 0.0f;
    mapPosition.z = 0.0f;

    state = STATE_LEVEL_INTRO;
    
    isScreenFinished = false;
    hasScratchPlayed = false;
}

static CollisionType CheckPlayerWallsCollision(Player *player, float deltaTime)
{
    Vector2 newPosition = GetPlayerNextPosition(player, deltaTime);
    int pixelX = newPosition.x > player->worldPosition.x ? floorf(newPosition.x) : ceilf(newPosition.x); 
    int pixelY = newPosition.y > player->worldPosition.y ? floorf(newPosition.y) : ceilf(newPosition.y);

    CollisionType collision = COLLISION_NO_COLLISION;
    //TODO: restrict the check in an area around the player
    for (int y = 0; (y < mapTexture.height && collision == COLLISION_NO_COLLISION) ; y++)
    {
        for (int x = 0; (x < mapTexture.width  && collision == COLLISION_NO_COLLISION); x++)
        {
            int pixelIndex = y*mapTexture.width + x;
            Color pixelColor = mapPixels[pixelIndex];
            if ( !COLOR_EQUAL(pixelColor, CPURPLE) &&       // Collision: white pixel, only check R channel
                (CheckCollisionCircleRec((Vector2){newPosition.x, newPosition.y}, player->radius,
                (Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
            {
                collision = COLLISION_WALL;
                //what kind of collision
                if(COLOR_EQUAL(pixelColor, CDARKRED)) collision = COLLISION_CONNECTION;
                if(COLOR_EQUAL(pixelColor, CORANGE))  collision = COLLISION_BADTRAIN;
                if(COLOR_EQUAL(pixelColor, BLACK))  collision = COLLISION_EXIT;
#if defined(_DEBUG)
    
                printf("Collision detected at : [%.2f:%.2f] (%d)%d, radius: %f\n",newPosition.x, newPosition.y, pixelIndex,collision, player->radius);
    
#endif
            }
        }
    }
    return collision;
}

void UnloadGameplayScreen(void)
{
    UnloadTexture(mapTexture);    // Unload cubicmap texture
    UnloadTexture(heartTexture);
    UnloadTexture(arrowTexture);
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
    if(state == STATE_LEVEL_INTRO)
    {
        UpdateDrawLevelIntro();
        return;
    }
    if(state == STATE_GAMEOVER)
    {
        UpdateDrawGameOver();
        return;
    }
    if(state == STATE_WON)
    {
        UpdateDrawWin();
        return;
    }
    if(state == STATE_FINISHED_LEVEL)
    {
        if(currentLevel < MAX_LEVELS)
        {
            UnloadLevel();
            LoadLevel(++currentLevel, &player);
            state = STATE_LEVEL_INTRO;
            return;
        }
        else
        {
            isGameOver = GAMEOVER_WON;
            state = STATE_WON;
            return;
        }
    }

    float deltaTime = GetFrameTime();
    
    levelTime -= deltaTime;

    UpdatePlayer(&player, deltaTime);
    CollisionType collision = CheckPlayerWallsCollision(&player, deltaTime);
    HandleCollision(collision, &player, deltaTime);

    if(player.healthPoints <= 0)
    {
        isGameOver = GAMEOVER_DEAD;
        state = STATE_GAMEOVER;
        return;
    }

    if(levelTime <= 0.0f && state == STATE_PLAYING)
    {
        isGameOver = GAMEOVER_TIMESUP;
        state = STATE_GAMEOVER;
        return;
    }
    


    UpdateCustomCamera(&player);

    BeginDrawing();
    ClearBackground(CBLUE);
    if(state == STATE_PLAYING)
    {
        UpdateMusicStream(tunnelVision);
        BeginMode3D(camera);
            DrawModel(model, mapPosition, 1.0f, WHITE);
        EndMode3D();
        DrawPlayer(&player);
        DrawUI(&player);
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
        if(line == 1) fscanf(levelFile, "%f", &levelMaxTime);
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
    player->rotationAngle = angle * (PI / 180);
    player->turnDirection = 0;
    player->speed = 1.0f;
    player->healthPoints = player->maxHealthPoints;
    player->graceTime = 0.0f;

    levelTime = levelMaxTime;
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

static void UnloadLevel(void)
{
    UnloadTexture(mapTexture);
    UnloadImageColors(mapPixels);
    UnloadModel(model);
}

static void UpdateCustomCamera(Player *player)
{
    camera.position.x = player->worldPosition.x;
    camera.position.z = player->worldPosition.y;

    camera.target.x = camera.position.x + (cosf(player->rotationAngle) * 50);
    camera.target.z = camera.position.z + (sinf(player->rotationAngle) * 50);
    //DrawLine(playerScreenPosition.x, playerScreenPosition.y, playerScreenPosition.x + (cosf(playerRotationAngle) * playerMinimapSize), playerScreenPosition.y + (sinf(playerRotationAngle) * playerMinimapSize), palette[7]);
}

static void HandleCollision(CollisionType type, Player *player, float deltaTime)
{
    if(type == COLLISION_NO_COLLISION)
    {
        Vector2 newPosition = GetPlayerNextPosition(player, deltaTime);
        player->worldPosition.x = newPosition.x;
        player->worldPosition.y = newPosition.y;
        return;
    }
    if(type == COLLISION_WALL || type == COLLISION_OBSTACLE)
    {
        if(player->graceTime <= 0)
        {
            float damage = player->speed * player->damageFactor;
            player->healthPoints -= damage;
            player->graceTime = player->maxGraceTime;
            float dmgTreshold = (player->maxSpeed * player->damageFactor) / 3;
            if(damage < dmgTreshold)
            {
                PlaySound(smallOuch);
            }
            else if (damage >= dmgTreshold && damage < dmgTreshold * 2)
            {
                PlaySound(bigOuch);
            }
            else if(damage >= dmgTreshold * 2)
            {
                PlaySound(OuhMother);
            }
            //TODO: handle hit
            return;
        }
    }
    if(type == COLLISION_EXIT)
    {
        isGameOver = GAMEOVER_LEFTSTATION;
        state = STATE_GAMEOVER;
        return;
    }
    if(type == COLLISION_BADTRAIN)
    {
        isGameOver = GAMEOVER_WRONGTRAIN;
        state = STATE_GAMEOVER;
        return;
    }
    if(type == COLLISION_CONNECTION)
    {
        state = STATE_FINISHED_LEVEL;
        return;
    }
}

static void UpdateDrawGameOver(void)
{
    if(IsMusicStreamPlaying(tunnelVision)) StopMusicStream(tunnelVision);
    if(IsSoundPlaying(smallOuch)) StopSound(smallOuch);
    if(IsSoundPlaying(bigOuch)) StopSound(bigOuch);
    if(IsSoundPlaying(OuhMother)) StopSound(OuhMother);
    if(!hasScratchPlayed)
    {
        PlaySound(scratch);
        hasScratchPlayed = true;
    }
    if(IsKeyPressed(KEY_SPACE))
    {
        isScreenFinished = true;
        return;
    }
    if(IsKeyPressed(KEY_R))
    {
        UnloadLevel();
        LoadLevel(currentLevel, &player);
        state = STATE_LEVEL_INTRO;
        return;
    }
    BeginDrawing();
    ClearBackground(CPURPLE);
    DrawText("GAME OVER",GetScreenWidth() / 2 - MeasureText("GAME OVER", 30) / 2, 50, 30, CORANGE);
    if(isGameOver == GAMEOVER_DEAD)
        DrawText("YOU DIED!",GetScreenWidth() / 2 - MeasureText("YOU DIED!", 30) / 2, 200, 30, CRED);
    else if(isGameOver == GAMEOVER_LEFTSTATION)
        DrawText("YOU LEFT THE STATION!",GetScreenWidth() / 2 - MeasureText("YOU LEFT THE STATION!", 30) / 2, 200, 30, CRED);
    else if(isGameOver == GAMEOVER_WRONGTRAIN)
        DrawText("YOU GOT ON THE WRONG TRAIN!",GetScreenWidth() / 2 - MeasureText("YOU GOT ON THE WRONG TRAIN!", 30) / 2, 200, 30, CRED);
    else if(isGameOver == GAMEOVER_TIMESUP)
        DrawText("YOU MISSED YOUR CONNECTION!",GetScreenWidth() / 2 - MeasureText("YOU GOT ON THE WRONG TRAIN!", 30) / 2, 200, 30, CRED);
    DrawText("Press [R] to restart the current level, [SPACE] exit to title", GetScreenWidth() / 2 - MeasureText("Press [R] to restart the current level, [SPACE] exit to title", 18) / 2, GetScreenHeight() - 40, 18, CBLUE);
    EndDrawing();
}

static void UpdateDrawLevelIntro(void)
{
    if(IsKeyPressed(KEY_SPACE))
    {
        state = STATE_PLAYING;
        PlayMusicStream(tunnelVision);
    }
    BeginDrawing();
    ClearBackground(CORANGE);
    const char *levelLabel = TextFormat("Level %d", currentLevel);
    DrawText(levelLabel, GetScreenWidth() / 2 - MeasureText(levelLabel, 30) / 2, 200, 30, CRED);
    DrawText("Press [SPACE] to continue", GetScreenWidth() / 2 - MeasureText("Press [SPACE] to continue", 24) / 2, GetScreenHeight() - 40, 24, CBLUE);
    EndDrawing();
}


static void UpdateDrawWin(void)
{
    if(IsMusicStreamPlaying(tunnelVision)) StopMusicStream(tunnelVision);
    
    if(IsKeyPressed(KEY_SPACE))
    {
        isScreenFinished = true;
    }
    BeginDrawing();
    ClearBackground(CORANGE);
    
    DrawText("YOU WON!", GetScreenWidth() / 2 - MeasureText("YOU WON!", 60) / 2, 200, 60, CRED);
    DrawText("Press [SPACE] to continue", GetScreenWidth() / 2 - MeasureText("Press [SPACE] to continue", 24) / 2, GetScreenHeight() - 40, 24, CBLUE);
    EndDrawing();
}
bool IsGameplayScreenFinished(void)
{
    return isScreenFinished;
}

static void DrawUI(Player *player)
{
    
        //DrawRectangle(GetScreenWidth()/2 - 52, 4, player.maxHealthPoints + 4, 44, CPURPLE);
        //DrawRectangle(GetScreenWidth()/2 - 50, 6, player.healthPoints, 40, CORANGE);
    DrawHP(player);
    DrawTimer(levelMaxTime, levelTime);
    DrawSpeed(player->maxSpeed, player->speed);
    DrawControls();
    return;
}

static void DrawHP(Player *player)
{
    int hearts = 3;
    float heartHPValue = player->maxHealthPoints / hearts;
    float scale = 0.5;
    float heartWidth = (float)heartTexture.width / 2 * scale;
    float hpWidth = heartWidth * hearts;
    float positionX = GetScreenWidth() / 2 - hpWidth / 2;
    float positionY = GetScreenHeight() - UIVerticalOffset - (float)heartTexture.height * scale;
    Rectangle source = { 0.0f, 0.0f, (float)heartTexture.width / 2 , (float)heartTexture.height };
    Rectangle sourceFull = {(float)heartTexture.width / 2, 0.0f, (float)heartTexture.width / 2 , (float)heartTexture.height };
    Rectangle dest = { positionX, positionY, heartWidth, (float)heartTexture.height * scale};
    Rectangle destFull = dest;
    Vector2 origin = { 0.0f, 0.0f };

    

    for (int i=0; i<hearts; i++)
    {
        
        dest.x += i > 0? heartWidth : 0;
        destFull.x = dest.x; 
        DrawTexturePro(heartTexture, source, dest, origin, 0.0f, WHITE);
        float hpRatio = player->healthPoints / player->maxHealthPoints;
        float hpToHearts = (hpWidth * hpRatio);
        
        destFull.width = Clamp(hpToHearts - (i * heartWidth), 0.0f, heartWidth);
        sourceFull.width = destFull.width * 2;

        DrawTexturePro(heartTexture, sourceFull, destFull, origin, 0.0f, WHITE);
    }
    return;
}

static void DrawTimer(float maxTime, float currentTime)
{
    float timeLeft = (currentTime / maxTime );
    timerEndAngle = timeLeft * 360 + timerStartAngle;
    Color timerColor = CYELLOW;
    if(timeLeft >= 0.5 && timeLeft <0.75) timerColor = CORANGE;
    if(timeLeft > 0.25 && timeLeft < 0.5) timerColor = CRED;
    if(timeLeft < 0.25) timerColor = CBURGUNDY;
    Vector2 center = {GetScreenWidth() / 2, timerRadius + UIVerticalOffset};
    DrawCircle(center.x, center.y, timerRadius, CBLUE);
    DrawCircleSector(center, timerRadius-2, timerStartAngle, timerEndAngle, (int)(timerEndAngle*10), timerColor);
    return;
}

static void DrawSpeed(float maxSpeed, float currentSpeed)
{
    DrawRectangle(UIHorizontalOffset, GetScreenHeight() / 2 - speedometerHeight / 2, speedometerWidth, speedometerHeight, CBLUE);
    float currentSpeedometerValue = (currentSpeed / maxSpeed) * speedometerHeight - 4;
    DrawRectangle(
        UIHorizontalOffset + 2, 
        (GetScreenHeight() / 2 - speedometerHeight / 2) + (speedometerHeight - currentSpeedometerValue), 
        speedometerWidth - 4, 
        currentSpeedometerValue - 2,
        CORANGE);
    return;
}
static void DrawControls(void)
{
    DrawTexturePro(
        arrowTexture, 
        (Rectangle){0,0,arrowTexture.width, arrowTexture.height},
        GetControlRectangle(LEFT),
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    DrawTexturePro(
        arrowTexture, 
        (Rectangle){0,0,-arrowTexture.width, arrowTexture.height},
        GetControlRectangle(RIGHT),
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    DrawTexturePro(
        arrowUpTexture, 
        (Rectangle){0,0,arrowTexture.width, arrowTexture.height},
        GetControlRectangle(UP),
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    DrawTexturePro(
        arrowUpTexture, 
        (Rectangle){0,0,arrowTexture.width, -arrowTexture.height},
        GetControlRectangle(DOWN),
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE
    );
    //DrawTextureEx(arrowTexture, (Vector2){GetScreenWidth() - arrowTexture.width, GetScreenHeight() - arrowTexture.height}, PI, 1.0f, WHITE);
    return;    
}