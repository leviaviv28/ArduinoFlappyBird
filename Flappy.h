#ifndef _Flappy_H
#define _Flappy_H

#include "ConsoleInterface.h"
#include <EEPROM.h>

#define TICKS_PER_SECOND 60
#define SKIP_TICKS 1800 / TICKS_PER_SECOND; // FPS
#define MAX_FRAMESKIP 1
#define WIDTH 480
#define HEIGHT 320

typedef struct {
  int x;
  int y;
  int w;
  int h;
  int y_vel;
  int score;
  bool dead;
}Player;


typedef struct {
  int x;
  int y;
  int width;
  int speed;
  int gap;
}Pipe;

void draw();
void draw_pipes();
void clear_pipes();
void draw_player();
void clear_player();
void draw_ground();
bool collision();
void awardScore();
void gameLoop();
void resetGame();
void resetGraphics();
void pipePhysics();
void player_physics();
#endif
