#ifndef ACTIONS_H
#define ACTIONS_H
#include "raylib.h"

typedef enum {
    CANCEL = 0,
    CONFIRM,
    LEFT,
    RIGHT,
    UP,
    DOWN
} Control;


//UI
static Rectangle recArrowLeft;
static Rectangle recArrowRight;
static Rectangle recArrowUp;
static Rectangle recArrowDown;
static Rectangle recConfirm;
static Rectangle recCancel;


Rectangle GetControlRectangle(Control control);
void SetControlRectangle(Control control, Rectangle rec);

// general actions
bool IsActionConfirm(void);
bool IsActionCancel(void);

// gameplay actions
bool IsActionTurnLeft(void);
bool IsActionTurnRight(void);
bool IsActionAccelerate(void);
bool IsActionDecelerate(void);
bool IsActionPause(void);


#endif