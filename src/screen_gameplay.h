#ifndef SCREEN_GAMEPLAY_H
#define SCREEN_GAMEPLAY_H
#include "raylib.h"
#include "constants.h"
#include "screens.h"
#include "player.h"

#define MAX_LEVELS 2

typedef enum {
    GAMEOVER_NOT_OVER = -1,
    GAMEOVER_DEAD = 0,
    GAMEOVER_TIMESUP,
    GAMEOVER_WRONGTRAIN,
    GAMEOVER_LEFTSTATION,
    GAMEOVER_WON
} GameOverReason;


typedef enum {
    COLLISION_NO_COLLISION = -1,
    COLLISION_WALL = 0,
    COLLISION_BADTRAIN,
    COLLISION_EXIT,
    COLLISION_OBSTACLE,
    COLLISION_CONNECTION
} CollisionType;

typedef enum {
    STATE_NOT_READY = -1,
    STATE_LEVEL_INTRO = 0,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_FINISHED_LEVEL,
    STATE_WON
} GameplayState;

static bool isScreenFinished = false;

static Vector3 mapPosition = { 0.0f, 0.0f, 0.0f };          // Set model position
static Model model;
static Texture2D mapTexture;

static Texture2D atlasTexture;
static Color *mapPixels; // for collisions

static Camera camera = { 0 };
static Player player;


static Sound smallOuch;
static Sound bigOuch;
static Sound OuhMother;
static Sound scratch;
static bool hasScratchPlayed;


static Music tunnelVision;

static GameOverReason isGameOver = GAMEOVER_NOT_OVER;

static int currentLevel = 0;
static Texture2D levelMapTexture;
static float levelTime = 0.0f;
static Vector3 levelSpawnPoint;
static float levelSpawnAngle;

static void UpdateCustomCamera(Player *player);
static void UpdateDrawGameOver(void);
static void UpdateDrawPause(void);
static void UpdateDrawLevelIntro(void);
static void UpdateDrawWin(void);

static void LoadLevel(int levelId, Player *player);
static void UnloadLevel(void);

static void HandleCollision(CollisionType type, Player *player, float deltaTime);
static CollisionType CheckPlayerWallsCollision(Player *player, float deltaTime);


static GameplayState state;
#endif