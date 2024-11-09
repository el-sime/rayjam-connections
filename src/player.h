#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct Player{
    Vector3 worldPosition;
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


    Texture2D animationTexture;
} Player;




void InitPlayer(Player *player);
void UpdateDrawPlayer(Player *player);


#endif