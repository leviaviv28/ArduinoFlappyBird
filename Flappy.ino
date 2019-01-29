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
bool scored = false;

Player player = {75, tft.width()/2, 17, 12, 3, 0, true};
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
  tft.setTextSize(5);
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
  tft.setCursor(220, 50);
  tft.print(player.score);
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
    for(int y = 0; y <= abs(player.y_vel) + 5; y++)
      for(int x = 0; x < player.w; x++)
        tft.drawPixel(player.x+x, player.y + player.w + player.y_vel + y, CYAN);
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
    scored = false;
  }
}

void player_physics(){
  if(collision()){
    player.dead = true;
    player.y_vel = 0;
  }else{
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
}

bool collision(){
  bool collided = false;
  //Check collision with floor
  if(player.y + player.h >= tft.height() - 10)
    collided = true;
  else if(player.x >= pipes.x && player.x <= pipes.x + pipes.width)
    if(player.y <= pipes.y || player.y - player.h >= pipes.y + pipes.gap)
      collided = true;
  return collided;
}

void awardScore(){
  if(pipes.x < player.x && not scored){
    tft.setTextColor(CYAN);
    tft.setCursor(220, 50);
    tft.print(player.score);
    tft.setTextColor(WHITE);
    player.score++;
    scored = true;
  }
}

void gameLoop(){
  loops = 0;
  while(!player.dead &&  millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      player_physics();
      pipePhysics();
      awardScore();
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
      if(player.dead){
        player.dead = false;
        resetGame();
      }else
        jump = true;
    }
  } else if(tapped){
    tapped = false;
  }
}

void resetGame(){
  resetGraphics();
  player.y = tft.height()/2 - player.h/2;
  player.score = 0;
  pipes.x = tft.width();
}

void resetGraphics(){
  //Clear player from screen
  for(int x = player.x; x <= player.x + player.w; x++)
      tft.drawFastVLine(x,player.y, player.h, CYAN);
  //Clear pipe from screen
  for(int i = 0; i <= pipes.width; i++){
      tft.drawFastVLine(pipes.x + i, 0, pipes.y + 1, CYAN);
      tft.drawFastVLine(pipes.x + i, pipes.y + pipes.gap - 1, 320, CYAN);
  }
  //Clear score from screen
  int score_digits = floor(log10(player.score)) + 1;
  for(int digit = 0; digit < score_digits; digit++)
    for(int x = 0; x < 25; x++)
      tft.drawFastVLine(220 + x + (digit * 30), 50, 35, CYAN);

  
}
