#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include "Flappy.h"


unsigned long next_game_tick;
int loops;


MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;

bool tapped = false;
bool jump = false;

Player player = {75, tft.width()/2, 17, 12, 3, 0};
Pipe pipes = Pipe{480, 100, 26, 7, 125};

uint16_t player_sprite[12][17] = { {CYAN,CYAN,CYAN,CYAN,CYAN,CYAN,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,CYAN,CYAN,CYAN,CYAN,CYAN},
                                   {CYAN,CYAN,CYAN,CYAN,BLACK,BLACK,LYELLOW,LYELLOW,LYELLOW,BLACK,WHITE,WHITE,BLACK,CYAN,CYAN,CYAN,CYAN},
                                   {CYAN,CYAN,CYAN,CYAN,BLACK,LYELLOW,LYELLOW,YELLOW,YELLOW,BLACK,WHITE,WHITE,WHITE,WHITE,BLACK,CYAN,CYAN},
                                   {CYAN,CYAN,BLACK,LYELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,LGREY,WHITE,WHITE,BLACK,WHITE,BLACK,CYAN,CYAN},
                                   {CYAN,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,LGREY,WHITE,WHITE,BLACK,WHITE,BLACK,CYAN,CYAN},
                                   {CYAN,BLACK,BLACK,BLACK,BLACK,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,LGREY,WHITE,WHITE,WHITE,BLACK,CYAN},
                                   {BLACK,LYELLOW,LYELLOW,LYELLOW,LYELLOW,LYELLOW,BLACK,YELLOW,YELLOW,YELLOW,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,CYAN},
                                   {BLACK,YELLOW,LYELLOW,LYELLOW,LYELLOW,YELLOW,BLACK,YELLOW,YELLOW,BLACK,RED,RED,RED,RED,RED,RED,BLACK},
                                   {CYAN,BLACK,BLACK,BLACK,BLACK,BLACK,ORANGE,ORANGE,BLACK,RED,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,CYAN},
                                   {CYAN,CYAN,BLACK,ORANGE,ORANGE,ORANGE,ORANGE,ORANGE,ORANGE,BLACK,RED,RED,RED,RED,RED,BLACK,CYAN},
                                   {CYAN,CYAN,CYAN,BLACK,BLACK,ORANGE,ORANGE,ORANGE,ORANGE,ORANGE,BLACK,BLACK,BLACK,BLACK,BLACK,CYAN,CYAN},
                                   {CYAN,CYAN,CYAN,CYAN,CYAN,BLACK,BLACK,BLACK,BLACK,BLACK,CYAN,CYAN,CYAN,CYAN,CYAN,CYAN,CYAN}};

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  tft.begin(0x9486);
  tft.setRotation(3);
  tft.fillScreen(CYAN);
  
}

void loop() {
  gameLoop();
}

void draw(){
  clear_pipes();
  draw_ground();
  draw_pipes();
  clear_player();
  draw_player();
  
 }

void draw_pipes(){
  //Draw Outlines
  tft.drawFastVLine(pipes.x, 0, pipes.y, BLACK); //Top left pipe outline
  tft.drawFastVLine(pipes.x + pipes.width, 0, pipes.y, BLACK); //Top right pipe outline
  tft.drawFastHLine(pipes.x, pipes.y, pipes.width, BLACK);//Top edge outline
  tft.drawFastVLine(pipes.x, pipes.y + pipes.gap, 320, BLACK); //Bottom left pipe outline
  tft.drawFastVLine(pipes.x + pipes.width, pipes.y + pipes.gap, 320, BLACK); //Bottom right pipe outline
  tft.drawFastHLine(pipes.x, pipes.y + pipes.gap - 1, pipes.width, BLACK);//Bottom edge outline
  //Fill Pipes
  for(int i = 1; i < pipes.width; i++){
    if(i > 4 && i < 9){
      tft.drawFastVLine(pipes.x + i, 0, pipes.y, GREEN);
      tft.drawFastVLine(pipes.x + i, pipes.y + pipes.gap, 320, GREEN);
    }else{
      tft.drawFastVLine(pipes.x + i, 0, pipes.y, DGREEN);
      tft.drawFastVLine(pipes.x + i, pipes.y + pipes.gap, 320, DGREEN);
    }
  }
}

void clear_pipes(){
  //Clear Outlines
  for(int i = 0; i <= pipes.speed; i++){
  tft.drawFastVLine(pipes.x + pipes.width + i, 0, pipes.y + 1, CYAN); //clears Top pipe outline
  tft.drawFastVLine(pipes.x + pipes.width + i, pipes.y + pipes.gap - 1, 320, CYAN); //clears bottom right pipe outline
  }
}

void draw_player(){
  for(int y = 0; y < player.h; y++)
    for(int x = 0; x < player.w; x++){
      tft.drawPixel(player.x+x, player.y+y, player_sprite[y][x]);
    }
}
void clear_player(){
  //tft.fillCircle(player.x, player.y - player.y_vel, player.radius, CYAN);
  if(player.y_vel > 0)
    for(int y = 0; y < player.y_vel; y++)
      for(int x = 0; x < player.w; x++)
        tft.drawPixel(player.x+x, player.y+y - player.y_vel, CYAN);
  else
    for(int y = 0; y <= abs(player.y_vel); y++)
      for(int x = 0; x < player.w; x++)
        tft.drawPixel(player.x+x, player.y + player.w - player.y_vel + y, CYAN);
}

void draw_ground(){
  int x, y;
  for(y = 0; y < 10; y++){
    tft.drawFastHLine(0,tft.height() - y, pipes.x, GREEN);
    tft.drawFastHLine(pipes.x + pipes.width, tft.height() - y, tft.width() - pipes.x + pipes.width, GREEN);
  }
}

void pipePhysics(){
  pipes.x -= pipes.speed;
  if(pipes.x + pipes.gap <= 0){
    pipes.x = 480;
    pipes.y = random(pipes.gap, 320 - pipes.gap);
  }
}

void player_physics(){
  player.y_vel++;
  if(player.y_vel > 10){
    player.y_vel = 10;
  }
  if(jump){
    jump = false;
    player.y_vel = -10;
  }
  player.y += player.y_vel;
  if(player.y > 320 - player.h){
    player.y = 320 - player.h;
  }else if (player.y < 0){
    player.y = 0;
  }
}

void gameLoop(){
  loops = 0;
  while( millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      player_physics();
      pipePhysics();
      next_game_tick += SKIP_TICKS;
      loops++;
  }
  draw();
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    if(not tapped){
      tapped = true;
      jump = true;
    }
  } else if(tapped){
    tapped = false;
  }
}
