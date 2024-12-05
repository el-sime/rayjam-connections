#include "raylib.h"
#include "actions.h"

// general actions
bool IsActionConfirm(void)
{

    //if(IsGestureDetected(GESTURE_TAP)) 
    return IsKeyPressed(KEY_SPACE); 
}
bool IsActionCancel(void)
{
    return IsKeyPressed(KEY_ESCAPE);
}

// gameplay actions
bool IsActionTurnLeft()
{
    if( 
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), recArrowLeft)
    ) return true;
    if(IsKeyDown(KEY_A)) return true;
    return false;
}
bool IsActionTurnRight()
{
    if( 
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), recArrowRight)
    ) return true;
    if(IsKeyDown(KEY_D)) return true;
    return false;
}
bool IsActionAccelerate()
{
    if( 
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), recArrowUp)
    ) return true;
    if(IsKeyDown(KEY_W)) return true;
    return false;
}
bool IsActionDecelerate()
{
    if( 
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), recArrowDown)
    ) return true;
    if(IsKeyDown(KEY_S)) return true;
    return false;
}
bool IsActionPause(void);

Rectangle GetControlRectangle(Control control)
{
    switch (control)
    {
    case CANCEL: return recCancel;
    case CONFIRM: return recConfirm;
    case LEFT: return recArrowLeft;
    case RIGHT: return recArrowRight;
    case UP: return recArrowUp;
    case DOWN: return recArrowDown;
    
    default:
        break;
    }
    Rectangle empty;
    return empty;
}


void SetControlRectangle(Control control, Rectangle rec)
{
    switch (control)
    {
    case CANCEL:
        recCancel = rec; 
        break;
    case CONFIRM: 
        recConfirm = rec; 
        break;
    case LEFT:
        recArrowLeft = rec; 
        break;
    case RIGHT: 
        recArrowRight = rec; 
        break;
    case UP:
        recArrowUp = rec; 
        break;
    case DOWN:
        recArrowDown = rec; 
        break;
    
    default:
        break;
    }
    return;
}