#ifndef _Flappy_H    // Put these two lines at the top of your file.
#define _Flappy_H    // (Use a suitable name, usually based on the file name.)

// Colours
#define BLACK   0x0000
#define DGREEN  0x0400
#define GREEN   0x07E0
#define CYAN    0x07FF
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define LYELLOW 0xFFF8
#define LGREY   0xCE59
#define RED     0xF208
#define ORANGE  0xFBA0
//Pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
//Touchscreen Values
#define TS_MINX 130
#define TS_MAXX 905
#define TS_MINY 75
#define TS_MAXY 930
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define TICKS_PER_SECOND 1
#define SKIP_TICKS 1 / TICKS_PER_SECOND; // FPS
#define MAX_FRAMESKIP 1

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
#endif // _Flappy_H    // Put this line at the end of your file.
