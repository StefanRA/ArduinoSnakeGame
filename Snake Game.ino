/*************************************************
Title: Snake Game
Author: Stefan Rapeanu-Andreescu
Date: 13.04.2017
*************************************************/

#include "LedControl.h"

#define DIN 12
#define CLK 11
#define CS 10
#define NUMBER_OF_USED_MAX7219 1

#define JOY_DELTA_X_PIN 0
#define JOY_DELTA_Y_PIN 1

#define INITIAL_GAME_DELAY 200
#define SCORE_DISPLAY_TIME 2000

LedControl ledControl = LedControl(DIN, CLK, CS, NUMBER_OF_USED_MAX7219);

int moveCounter = 0;

int xAxis = 0;
int yAxis = 0;

String direction;

int snakeX[64];
int snakeY[64];

int gameDelay = INITIAL_GAME_DELAY;

int snakeSize;

int foodX;
int foodY;

int score;

boolean gameIsRunning = false;

const unsigned char CH[10][4] = {
  {B00111110, B01000001, B01000001, B00111110}, // 0
  {B01000010, B01111111, B01000000, B00000000}, // 1
  {B01100010, B01010001, B01001001, B01000110}, // 2
  {B00100010, B01000001, B01001001, B00110110}, // 3
  {B00011000, B00010100, B00010010, B01111111}, // 4
  {B00100111, B01000101, B01000101, B00111001}, // 5
  {B00111110, B01001001, B01001001, B00110000}, // 6
  {B01100001, B00010001, B00001001, B00000111}, // 7
  {B00110110, B01001001, B01001001, B00110110}, // 8
  {B00000110, B01001001, B01001001, B00111110}  // 9
};

void setup()
{
  ledControl.shutdown(0, false);
  ledControl.setIntensity(0, 8);
  ledControl.clearDisplay(0);
  Serial.begin(9600);
  
  newGame();
}

void loop()
{
  if (gameIsRunning)
  {
    ledControl.clearDisplay(0);
      
    xAxis = simple(analogRead(JOY_DELTA_X_PIN));
    yAxis = simple(analogRead(JOY_DELTA_Y_PIN));
      
    if (yAxis > 5 && direction != "up")
    {
      direction = "down";
    }
    else if (yAxis < 3 && direction != "down")
    {
      direction = "up";
    }
    else if (xAxis > 5 && direction != "left")
    {
      direction = "right";
    }
    else if (xAxis < 3 && direction != "right")
    {
      direction = "left";
    }
    
    move(direction);
  
    checkIfHitFood();
    checkIfHitSelf();

    respawnFoodIfNecessary();
    
    drawSnake();
    drawFood();
    
    delay(gameDelay);
  }
}

void respawnFoodIfNecessary()
{
  if (score > 5)
  {
    if (moveCounter >= 25)
    {
      spawnNewFood();
      moveCounter = 0;
    }
    else
    {
      ++moveCounter;
    }
  }
}

int simple(int num)
{
  return (num * 9 / 1024);
}

void move(String movingDirection)
{
  for (int i = snakeSize - 1; i > 0; --i)
  {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }

  if (movingDirection == "up")
  {
    if (snakeY[0] == 0)
    {
      snakeY[0] = 7;
    }
    else
    {
      --snakeY[0];
    }
  }
  else if (movingDirection == "down")
  {
    if (snakeY[0] == 7)
    {
      snakeY[0] = 0;
    }
    else
    {
      ++snakeY[0];
    }
  }
  else if (movingDirection == "left") 
  {
    if (snakeX[0] == 0)
    {
      snakeX[0] = 7;
    }
    else
    {
      --snakeX[0];
    }
  }
  else if (movingDirection == "right")
  {
    if (snakeX[0] == 7)
    {
      snakeX[0] = 0;
    }
    else
    {
      ++snakeX[0];
    }
  }
}

void drawSnake()
{
  for (int i = 0; i < snakeSize; i++)
  {
    ledControl.setLed(0, snakeY[i], snakeX[i], true);
  }
}

void drawFood()
{
  ledControl.setLed(0, foodY, foodX, true);
  delay(50);
  ledControl.setLed(0, foodY, foodX, false);
}

void spawnNewFood()
{
  int spawnNewFoodX = random(0, 8);
  int spawnNewFoodY = random(0, 8);
  
  while (isSnake(spawnNewFoodX, spawnNewFoodY))
  {
    spawnNewFoodX = random(0, 8);
    spawnNewFoodY = random(0, 8);
  }
  
  foodX = spawnNewFoodX;
  foodY = spawnNewFoodY;
}

void checkIfHitFood()
{
  if (snakeX[0] == foodX && snakeY[0] == foodY)
  {
    ++score;
    ++snakeSize;
    decreaseGameDelay();
    spawnNewFood();
  }
}

void checkIfHitSelf()
{
  for (int i = 1; i < snakeSize - 1; ++i)
  {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
    {
      gameOver();
    }
  }
}

boolean isSnake(int x, int y)
{
  for (int i = 0; i < snakeSize - 1; ++i)
  {
    if ((x == snakeX[i]) && (y == snakeY[i]))
    {
      return true;
    }
  }
  return false;
}

void decreaseGameDelay()
{
  if (gameDelay >= 50)
  {
    gameDelay -= 10;
  }
}

void newGame()
{
  gameDelay = INITIAL_GAME_DELAY;
  score = 0;
  for (int i = 0; i < 64; ++i)
  {
    snakeX[i] = -1;
    snakeY[i] = -1;
  }

  snakeX[0] = 4;
  snakeY[0] = 8;
  direction = "up";
  snakeSize = 1;
  spawnNewFood();
  gameIsRunning = true;
}

void gameOver()
{
  gameIsRunning = false;
  displayLightShow();
  displayScore();
  score = 0;
  newGame();
}

void displayLightShow()
{
  for (int x = 0; x < 8; ++x)
  {
    for (int y = 0; y < 8; ++y)
    {
      ledControl.setLed(0, y, x, true);
      delay(20);
      ledControl.setLed(0, y, x, false);
    }
  }
}

void displayScore()
{
  int first = score / 10;
  int second = score % 10;
  char i = 0;
  while (i < 4)
  {
    ledControl.setRow(0, i, CH[first][i]);
    ++i;
  }
  i = 0;
  while (i < 4)
  {
    ledControl.setRow(0,i+4,CH[second][i]);
    ++i;
  }
  delay(SCORE_DISPLAY_TIME);
}
