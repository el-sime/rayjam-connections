#ifndef SCREENS_H
#define SCREENS_H
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_GAMEOVER
} GameScreen;

/********** GLOBALS **********/
extern GameScreen currentGameScreen;
extern Color palette[MAX_PALETTE_COLORS];

/******** LOGO SCREEN ********/

void UpdateDrawLogoScreen(void);
bool IsScreenLogoFinished(void);

/****** GAMEPLAY SCREEN ******/

void UpdateDrawGameplay(void);

/*****************************/
#endif