#include "Flappy.h"

ConsoleInterface console;
unsigned long next_game_tick;
int loops;

bool tapped = false;
bool jump = false;
bool scored = false;
int cur_highScore = EEPROM.read(0);

Player player = {75, HEIGHT/2, 25, 25, 0, 0, false};
Pipe pipes = Pipe{480, 100, 26, 15, 125};

byte flappySprite[25] = {CLEAR,YELLOW,WHITE,YELLOW,CLEAR,
                         YELLOW,YELLOW,WHITE,BLACK,YELLOW,
                         YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,
                         YELLOW,BLACK,BLACK,RED,RED,
                         CLEAR,YELLOW,YELLOW,YELLOW,CLEAR};

void setup() {
  randomSeed(analogRead(0));
  console.screen.storeSprite(flappySprite, 0);
  console.screen.fillScreen(CYAN);
  drawMainMenu();
}

void loop() {
  gameLoop();
}

void drawMainMenu(){
  drawGround();
  while(console.btnArr[0] != 1){
    //Waiting to start game
    console.getInputs();
    clearPipes();
    pipePhysics();
    drawPipes();
    console.screen.drawText(10, 100, 7, BLACK, "Crappy Bird");
    console.screen.drawText(140, 190, 2, BLACK, "Press A to start");
  }
  resetGame();
}

void draw(){
  drawPipes();
  //Draw player
  console.screen.drawSprite(0, player.x, player.y, 5);
  //Draw score
  char str[3];
  itoa(player.score, str, 10);
  console.screen.drawText(220, 50, 5, 1, str);
 }

void drawPipes(){
  for(int i = 0; i < pipes.speed; i++){
    console.screen.drawFastVLine(pipes.x - i, 0, pipes.y, DARKGREEN);
    console.screen.drawFastVLine(pipes.x - i, pipes.y + pipes.gap, HEIGHT - (pipes.y + pipes.gap) - 20, DARKGREEN);
  }
}

void clearPipes(){
  for(int i = pipes.speed; i > 0; i--){
    console.screen.drawFastVLine(pipes.x + pipes.width + i, 0, pipes.y, CYAN);
    console.screen.drawFastVLine(pipes.x + pipes.width + i, pipes.y + pipes.gap, HEIGHT - (pipes.y + pipes.gap) - 20, CYAN);
  }
}

void clearPlayer(){
  if(player.y_vel > 0){
    for(int i = 0; i < 10; i++) {
      console.screen.drawFastHLine(player.x, player.y - i, player.w, CYAN);
    }
  } else if (player.y_vel < 0) {
    for(int i = 0; i < 10; i++ ){
      console.screen.drawFastHLine(player.x, player.y + player.h + i, player.w, CYAN);
    }
  }
}

void drawGround(){
  console.screen.fillRect(0, 300, 480, 20, GREEN);
}

void pipePhysics(){
  pipes.x -= pipes.speed;
  if(pipes.x + pipes.gap <= 0){
    pipes.x = 480;
    pipes.y = random(pipes.gap, HEIGHT - pipes.gap - 40);
    scored = false;
  }
}

void playerPhysics(){
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
  if(player.y + player.h >= HEIGHT - 20){
    collided = true;
  }
  //Check for collision with pipes
  else if(player.x + player.w > pipes.x && player.x < pipes.x + pipes.width && (player.y < pipes.y || player.y + player.h > pipes.y + pipes.gap)){
    collided = true;
  }
  return collided;
}

void awardScore(){
  char str[3];
  if(pipes.x < player.x && not scored){
    //Clear score text
    itoa(player.score, str, 10);
    console.screen.drawText(220, 50, 5, CYAN, str);
    //Write new score text
    itoa(++player.score, str, 10);
    console.screen.drawText(220, 50, 5, BLACK, str);
    scored = true;
  }
}

void gameLoop(){
  clearPipes();
  clearPlayer();
  loops = 0;
  while(!player.dead &&  millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      pipePhysics();
      console.getInputs();
      if (console.btnArr[0] == 1) {
        if(!tapped){
          tapped = true;
          jump = true;
        }
      } else if(tapped) {
        tapped = false;
      }
      playerPhysics();
      awardScore();
      next_game_tick += SKIP_TICKS;
      loops++;
  }
  if(player.dead){
    drawGameOver();
    console.getInputs();
    if(console.btnArr[0] == 1){
      resetGame();
    }
  } else {
    draw();
  }
}

void drawGameOver(){
  char str[3];
  itoa(player.score, str, 10);
  console.screen.drawText(220, 50, 5, CYAN, str);
  console.screen.drawText(60, 50, 7, BLACK, "Game Over");
  console.screen.drawText(130, 130, 5, BLACK, "Score:");
  console.screen.drawText(300, 135, 5, BLACK, str);
  if(player.score >= cur_highScore){
      EEPROM.write(0, player.score);
      cur_highScore = player.score;
      console.screen.drawText(40, 190, 5, RED, "New Highscore!");
  } else {
    console.screen.drawText(65, 190, 5, BLACK, "Highscore:");
    itoa(cur_highScore, str, 10);
    console.screen.drawText(355, 190, 5, BLACK, str);
  }

}

void resetGame(){
  player.dead = false;
  player.y = HEIGHT/2 - player.h/2;
  player.score = 0;
  pipes.x = WIDTH;
  resetGraphics();
}

void resetGraphics(){
  console.screen.fillScreen(CYAN);
  drawGround();
  draw();  
}
