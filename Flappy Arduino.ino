#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

// Colours
#define BLACK   0x0000
#define BLUE    0x001F
#define DGREEN  0x87F0
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
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

typedef struct {
  int x;
  int y;
  int radius;
  int y_vel;
  int score;
}Player;


MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;
bool tapped = false;
bool jump = false;
bool scored = false;
bool started = false;
Player player = {75, tft.width()/2, 10, 3};
int pipe_x = 200;
int pipe_width = 30;
int pipe_gap = 250;
int pipe_speed = 10;


const int TICKS_PER_SECOND = 30;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 15;
unsigned long next_game_tick;
int loops;

void playerPhysics(){
  player.y_vel++;
  if(player.y_vel > 10){
    player.y_vel = 10;
  }
  if(jump){
    jump = false;
    player.y_vel = -10;
  }
  player.y += player.y_vel;
  if(player.y > 320 - player.radius){
    player.y = 320 - player.radius;
  }else if (player.y < 0){
    player.y = 0;
  }
}

void pipePhysics(){
  pipe_x -= pipe_speed;
  if(pipe_x + 75 <= 0){
    pipe_x = tft.width();
    pipe_gap = random(50, 200);
    scored = false;
  }
}

void drawPlayer(){
  tft.fillCircle(player.x, player.y, player.radius, BLUE);
}

void clearPlayer(){
  tft.fillCircle(player.x, player.y - player.y_vel, player.radius, CYAN);
}

void drawPipes(){
  tft.drawFastVLine(pipe_x, 0, pipe_gap, BLACK);
  tft.drawFastVLine(pipe_x + pipe_width + 15, 0, pipe_gap, BLACK);
  tft.drawFastVLine(pipe_x, pipe_gap + 75, pipe_gap + 75, BLACK);
  tft.drawFastVLine(pipe_x + pipe_width + 15, pipe_gap + 75, 320 - pipe_gap, BLACK);
  for(int i = 1; i < pipe_width - 1; i++){
    tft.drawFastVLine(pipe_x + i, 0, pipe_gap, GREEN);
    tft.drawFastVLine(pipe_x + i, pipe_gap + 75, 320 - pipe_gap, GREEN);
  }
  for(int i = -10; i < pipe_width + 3; i++){
    tft.drawFastVLine(pipe_x + i, pipe_gap, 15, GREEN);
    //tft.drawFastVLine(pipe_x + i, pipe_gap + 75, 320 - pipe_gap, GREEN);
  }
}

void clearPipes(){
  for(int i = pipe_width - pipe_speed; i < pipe_width; i++){
    tft.drawFastVLine(pipe_x + i + pipe_width, 0, pipe_gap, CYAN);
    tft.drawFastVLine(pipe_x + i + pipe_width, pipe_gap + 75, 320 - pipe_gap, CYAN);
  }
  for(int i = pipe_width + pipe_speed; i < pipe_width + 3; i++){
    tft.drawFastVLine(pipe_x + i, pipe_gap, 15, CYAN);
    //tft.drawFastVLine(pipe_x + i, pipe_gap + 75, 320 - pipe_gap, GREEN);
  }
}

void draw(){
  clearPipes();
  clearPlayer();
  drawPlayer();
  drawPipes();
  tft.setCursor(220, 50);
  tft.print(player.score);
}

void awardScore(){
  if(pipe_x < player.x && not scored){
    tft.setTextColor(CYAN);
    tft.setCursor(220, 50);
    tft.print(player.score);
    tft.setTextColor(WHITE);
    player.score++;
    scored = true;
  }
}

void mainMenu(){
  p = ts.getPoint(); 
  if((p.z > MINPRESSURE ) && (p.z < MAXPRESSURE)){
  started = true;
  next_game_tick = millis();
  }
  tft.fillScreen(CYAN);
  tft.setCursor(240, 160);
  tft.setTextColor(WHITE);
  tft.setTextSize(5);
}


void gameLoop(){
  loops = 0;
  while( millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      playerPhysics();
      pipePhysics();
      next_game_tick += SKIP_TICKS;
      loops++;
  }
  awardScore();
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
void setup() {
  Serial.begin(9600);
  tft.begin(0x9486);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.setCursor(20, 120);
  tft.setTextColor(WHITE);
  tft.setTextSize(7);
  tft.println("Tappy Bird");
  tft.setTextSize(2);
  tft.setCursor(160, 190);
  tft.println("Tap To Start");
}

void loop() {
  if(started){
    gameLoop();
  } else {
    mainMenu();
  }
}
