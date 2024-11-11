#include "raylib.h"
#include "constants.h"
#include "screens.h"
#include "screen_gameplay.h"

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
    DrawText("GAMEPLAY!", 30, 30, 30, BLACK);
}