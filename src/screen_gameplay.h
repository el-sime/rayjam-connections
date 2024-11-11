#ifndef SCREEN_GAMEPLAY_H
#define SCREEN_GAMEPLAY_H
#include "raylib.h"
#include "constants.h"
#include "screens.h"
#include "player.h"

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



static Vector3 mapPosition = { 0.0f, 0.0f, 0.0f };          // Set model position
static Model model;
static Texture2D mapTexture;


static Texture2D atlasTexture;
static Color *mapPixels; // for collisions



static Camera camera = { 0 };
static Player player;

static CollisionType CheckPlayerWallsCollision(Player *player);



static Music tunnelVision;

static GameOverReason isGameOver = NOT_OVER;


static int currentLevel = 0;
static Texture2D levelMapTexture;
static float levelTime = 0.0f;

static void UpdateCustomCamera(void);
static void UpdateDrawGameOver(void);

static void UpdateDrawPauseScreen(void);

#endif