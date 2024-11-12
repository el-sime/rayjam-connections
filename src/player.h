#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct Player{
    Vector3 worldPosition;
    float radius;
    float speed;
    float minSpeed;
    float maxSpeed;
    float acceleration;
    float rotationAngle;
    int turnDirection;
    float turnSpeed;
    int animationFrame;
    float animationFrameWidth;
    int animationMaxFrames;
    float animationFrameDuration;
    float animationTime;
    float maxGraceTime;
    float graceTime;
    float healthPoints;
    float maxHealthPoints;
    float damageFactor;
    Texture2D animationTexture;
} Player;




void InitPlayer(Player *player);
void UpdatePlayer(Player *player, float deltaTime);
void HandlePlayerInput(Player *player, float deltaTime);
void DrawPlayer(Player *player);
Vector2 GetPlayerNextPosition(Player *player, float deltaTime);
#endif