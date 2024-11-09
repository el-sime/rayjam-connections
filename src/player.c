#include "raylib.h"
#include "player.h"

void InitPlayer(Player *player)
{
    player->worldPosition = (Vector3){0.0f, 0.0f, 0.0f};
    player->speed = 0.0f;
    player->minSpeed = 1.0f;
    player->maxSpeed = 5.0f;
    player->acceleration = 10.0f;

    player->rotationAngle = 0.0f;
    player->turnDirection = 0;
    player->turnSpeed = 90 * (PI / 180);

    player->animationTexture = LoadTexture("resources/player_spritesheet.png");
    player->animationMaxFrames = 5;
    player->animationFrameDuration = 0.3;
    player->animationFrameWidth = player->animationTexture.width / player->animationMaxFrames;
    player->animationFrame = 0;
    player->animationTime = 0.0f;

    player->maxGraceTime = 1.5f;
    player->graceTime = 0.0f;
    player->maxHealthPoints = 100.0f;
    player->healthPoints = player->maxHealthPoints;
    return player;
}

void UpdateDrawPlayer(Player *player)
{
    float deltaTime = GetFrameTime();
    if (player->graceTime > 0)
    {
        player->graceTime -= deltaTime;
    }

    if(IsKeyPressed(KEY_A))
    {
        player->turnDirection = -1;
    }
    if(IsKeyPressed(KEY_D))
    {
        player->turnDirection = 1;
    }
    if(
        (player->turnDirection == -1 && IsKeyReleased(KEY_A)) ||
        (player->turnDirection == 1 && IsKeyReleased(KEY_D))
    )
    {
        player->turnDirection = 0;
    }

    if(IsKeyDown(KEY_W))
    {
        player->speed = Clamp(player->speed + player->acceleration * deltaTime, player->minSpeed, player->maxSpeed);
    }
    if(IsKeyDown(KEY_S))
    {
        player->speed = Clamp(player->speed - player->acceleration * deltaTime, player->minSpeed, player->maxSpeed);
    }

    player->animationTime += deltaTime;
    
    if(player->speed >= 1   && player->speed < 2.5) player->animationFrameDuration = 0.3;
    if(player->speed >= 2.5 && player->speed < 3.5) player->animationFrameDuration = 0.2;
    if(player->speed >= 3.5 && player->speed <= 5)  player->animationFrameDuration = 0.1;

    if(player->animationTime >= player->animationFrameDuration)
    {
        player->animationTime = 0.0f;
        if(player->animationFrame < player->animationMaxFrames -1)
        {
            player->animationFrame++;
        }
        else
        {
            player->animationFrame = 0;
        }
    }

    Rectangle sourceRec = {
        player->animationFrame * player->animationFrameWidth,
        0,
        player->animationFrameWidth,
        player->animationTexture.height
    };

    Rectangle destinationRec = {
        GetScreenWidth() /2 - player->animationFrameWidth,
        GetScreenHeight() - player->animationTexture.height * 3,
        player->animationFrameWidth * 3,
        player->animationTexture.height * 3
    };

    DrawTexturePro(
        player->animationTexture,
        sourceRec,
        destinationRec,
        (Vector2){0,0},
        0.0f,
        WHITE
    );
}