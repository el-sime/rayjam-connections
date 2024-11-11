#ifndef SCREENS_H
#define SCREENS_H
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_GAMEOVER
} GameScreen;

/********** GLOBALS **********/
extern Color palette[MAX_PALETTE_COLORS];

/******** LOGO SCREEN ********/
void UpdateDrawLogoScreen(void);
bool IsLogoScreenFinished(void);

/******* TITLE  SCREEN *******/
void InitTitleScreen(void);
void UpdateDrawTitleScreen(void);
bool IsTitleScreenFinished(void);

// /****** GAMEPLAY SCREEN ******/
void InitGameplayScreen(void);
void UpdateDrawGameplayScreen(void);
bool IsGameplayScreenFinished(void);
void UnloadGameplayScreen(void);
/*****************************/
#endif