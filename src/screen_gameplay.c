#include "raylib.h"
#include "constants.h"
#include "screens.h"
#include "player.h"

static Camera camera = { 0 };
static Player player;

void InitGameplayScreen(void)
{
    camera.position = (Vector3){ 0.0f, 0.4f, 0.0f };
    camera.target = (Vector3){ 2.0f, 0.4f, 15.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    InitPlayer(&player);
}