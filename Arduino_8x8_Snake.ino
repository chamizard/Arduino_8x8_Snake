/*
 * A snake game created for Ardunio Mega 2560 by Cameron Mann.
 * Uses Segment library also created by Cameron Mann.
 * Uses LedControl library from Elegoo in their Ardunio Mega 2560 Complete kit.
 * Also makes use of a Joystick Module and MAX7219 LED Dot Matrix Module.
  */

#include <Segment.h>

#include <LedControl.h>

LedControl lc = LedControl(12, 10, 11, 1);

// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

// Min/max values for analog stick
const int STICK_MAX = 1024;
const int STICK_MIN = 0;
const int DEADZONE = 100;

// Size of the Led Matrix
const int MATRIX_ROW_SIZE = 8;
const int MATRIX_COL_SIZE = 8;

// Constants for stick direciton
const int LEFT = 0;
const int RIGHT = 1;
const int UP = 2;
const int DOWN = 3;

// Constants for game state
const int START = 0;
const int PLAYING = 1;
const int PAUSED = 2;
const int GAMEOVER = 3;

// List of snake segments
Segment *segList[MATRIX_ROW_SIZE * MATRIX_COL_SIZE];
// Variables for player location, apple location, player score, size of snake (0 means only head segment)
int playerX, playerY, appleX, appleY, score, snakeSize;
// Direction of analog stick
int stickDirection;
// Array representing state of the 'game board', 1 for occupied spaces, 0 for unoccupied
int BOARD[MATRIX_ROW_SIZE][MATRIX_COL_SIZE];
// Represents the state of the game using the constants for game state
int gameState;
bool paused;

// Constant for size of letter arrays
const int LETTER_SIZE = 3;
// Byte arrays for letters that are displayed
byte p[LETTER_SIZE] = {B00111000, B00101000, B00111110};
byte a[LETTER_SIZE] = {B00111110, B00101000, B00111110};
byte u[LETTER_SIZE] = {B00111110, B00000010, B00111110};
byte s[LETTER_SIZE] = {B00101110, B00101010, B00111010};
byte e[LETTER_SIZE] = {B00101010, B00101010, B00111110};
byte d[LETTER_SIZE] = {B00011100, B00100010, B00111110};
byte g[LETTER_SIZE] = {B00101110, B00100110, B00111100};
byte m[LETTER_SIZE] = {B00111110, B00111000, B00111110};
byte o[LETTER_SIZE] = {B00111110, B00100010, B00111110};
byte v[LETTER_SIZE] = {B00111100, B00000110, B00111100};
byte r[LETTER_SIZE] = {B00111010, B00101100, B00111110};
byte c[LETTER_SIZE] = {B00110110, B00100010, B00111110};
byte y[LETTER_SIZE] = {B00111000, B00001110, B00111000};
byte colon[LETTER_SIZE] = {B00000000, B00010100, B00000000};
byte zero[LETTER_SIZE] = {B00111110, B00100010, B00111110};
byte one[LETTER_SIZE] = {B00000010, B00111110, B00010010};
byte two[LETTER_SIZE] = {B00111010, B00101010, B00100110};
byte three[LETTER_SIZE] = {B00111110, B00101010, B00101010};
byte four[LETTER_SIZE] = {B00111110, B00001000, B00111000};
byte five[LETTER_SIZE] = {B00101100, B00101010, B00111010};
byte six[LETTER_SIZE] = {B00101100, B00101010, B00011100};
byte seven[LETTER_SIZE] = {B00111110, B00100000, B00100000};
byte eight[LETTER_SIZE] = {B00111110, B00101010, B00111110};
byte nine[LETTER_SIZE] = {B00111110, B00101000, B00111000};
byte space[LETTER_SIZE] = {B00000000, B00000000, B00000000};
//  Arrays of letters combined into words
byte *wordPaused[6] = {p, a, u, s, e, d};
byte *wordGameOver[8] = {g, a, m, e, o, v, e, r};
byte *wordScore[6] = {s, c, o, r, e, colon};
byte *wordReadyGo[8] = {r, e, a, d, y, space, g, o};
// Pointer to first and last segment of the snake
Segment *head;
Segment *tail;

void randomLocation() {
  int col, row;
  col = random(0, 8);
  row = random(0, 8);
  while (BOARD[row][col] == 1) {
    col = random(0, 8);
    row = random(0, 8);
  }
  appleX = row;
  appleY = col;
}

void setup() {
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
  playGame();
}

void initGame() {
  playerX, playerY, score, snakeSize = 0;
  stickDirection = LEFT;
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);
  head = new Segment(playerX, playerY);
  tail = head;
  gameState = START;
  paused = false;
}

// TODO: Add deadzone for so stick doesn't have to be fully pressed in one direction
void getDirection() {
  int x = analogRead(X_pin);
  int y = analogRead(Y_pin);
  paused = !digitalRead(SW_pin);
  if (x <= STICK_MAX && x >= STICK_MAX - DEADZONE) { // Left direction
    stickDirection = LEFT;
    return;
  }
  if (x >= STICK_MIN && x <= STICK_MIN + DEADZONE) { // Right direction
    stickDirection = RIGHT;
    return;
  }
  if (y >= STICK_MIN && y <= STICK_MIN + DEADZONE) { // Up direction
    stickDirection = UP;
    return;
  }
  if (y <= STICK_MAX && y >= STICK_MAX - DEADZONE) { // Down direction
    stickDirection = DOWN;
    return;
  }
}

void doMove(int dir) {
  int tmp_x = playerX;
  int tmp_y = playerY;

  if (dir == LEFT) {
    playerX++;
    if (playerX >= MATRIX_ROW_SIZE) {
      playerX = 0;
    }
  }
  if (dir == RIGHT) {
    playerX--;
    if (playerX == -1) {
      playerX = MATRIX_ROW_SIZE - 1;
    }
  }
  if (dir == UP) {
    playerY--;
    if (playerY == -1) {
      playerY = MATRIX_COL_SIZE - 1;
    }
  }
  if (dir == DOWN) {
    playerY++;
    if (playerY >= MATRIX_COL_SIZE) {
      playerY = 0;
    }
  }
  head->updateSelf(playerX, playerY);
  if (snakeSize > 0) {
    head->updateNext();
  }
  for (int i = 0; i < snakeSize - 1; i++) {
    segList[i]->updateNext();
  }
  lc.clearDisplay(0);
  lc.setLed(0, appleX, appleY, true);
  delay(0);
  lc.setLed(0, head->getX(), head->getY(), true);
  for (int i = 0; i < snakeSize; i++) {
    lc.setLed(0, segList[i]->getX(), segList[i]->getY(), true);
  }
  delay(200);
}

//TODO: add collision for snake segments
void doCollision() {
  // player scores
  if (playerX == appleX && playerY == appleY) {
    score++;
    addSegment();
    BOARD[appleX][appleY] = 0;
    randomLocation();
  } else {
    for (int i = 0; i < snakeSize; i++) {
      if (playerX == segList[i]->getX() && playerY == segList[i]->getY()) {
        gameState = GAMEOVER;
      }
    }
  }
}

void addSegment() {
  Segment *tmp = new Segment(0, 0);
  segList[snakeSize] = tmp;
  if (snakeSize == 0) {
    head->setNext(tmp);
  } else {
    segList[snakeSize - 1]->setNext(tmp);
  }
  tail = tmp;
  snakeSize++;
}

// Needs to be fixed to clear board positions before updating
void updateBoard() {
  for (int i = 0; i < MATRIX_ROW_SIZE; i++) {
    for (int j = 0; j < MATRIX_COL_SIZE; j++) {
      BOARD[i][j] = 0;
    }
  }
  BOARD[playerX][playerY] = 1;
  for (int i = 0; i < snakeSize; i++) {
    BOARD[segList[i]->getX()][segList[i]->getY()] = 1;
  }
  BOARD[appleX][appleY] = 1;
}

void printBoard() {
  Serial.print("NOTE BOARD IS MIRRORED \n");
  for (int i = MATRIX_ROW_SIZE - 1; i >= 0; i--) {
    for (int j = MATRIX_COL_SIZE - 1; j >= 0; j--) {
      Serial.print(BOARD[i][j]);
    }
    Serial.print("\n");
  }
}

void displayLetter(byte letter[], int pos) {
  for (int i = 0; i < LETTER_SIZE; i++) {
    lc.setRow(0, pos++, letter[i]);
  }
}

void displayMessage(byte *wrd[], int wrdSize, int delayTime = 750) {
  int firstLetterPos = 5;
  int secondLetterPos = 1;
  lc.clearDisplay(0);
  for (int i = 0; i < wrdSize; i++) {
    if (i % 2 == 0) {
      displayLetter(wrd[i], firstLetterPos);
    } else {
      displayLetter(wrd[i], secondLetterPos);
      delay(delayTime);
    }
  }
}

void pause() {
  displayMessage(wordPaused, 6);
  while (1) {
    delay(200); // Delay to wait for input
    // do nothing
    if (!digitalRead(SW_pin)) {
      break;
    }
  }
  paused = false;
}

// TODO: consider adding a function that does the LED displaying/updating
void playGame() {
  initGame();
  if (gameState == START) {
    displayMessage(wordReadyGo, 8, 2000);
    randomLocation();
    gameState = PLAYING;
  }
  while (gameState == PLAYING) {
    getDirection();
    if (paused) {
      pause();
    }
    doMove(stickDirection);
    doCollision();
    updateBoard();
  }
  if (gameState == GAMEOVER) {
    lc.clearDisplay(0);
    displayMessage(wordGameOver, 8, 2000);
    displayMessage(wordScore, 6, 1000);
    int firstScoreDigit = score / 10;
    int secondScoreDigit = score % 10;
    byte *digitOnePtr, *digitTwoPtr;
    switch (firstScoreDigit) {
      case 0: digitOnePtr = zero;
        break;
      case 1: digitOnePtr = one;
        break;
      case 2: digitOnePtr = two;
        break;
      case 3: digitOnePtr = three;
        break;
      case 4: digitOnePtr = four;
        break;
      case 5: digitOnePtr = five;
        break;
      case 6: digitOnePtr = six;
        break;
      case 7: digitOnePtr = seven;
        break;
      case 8: digitOnePtr = eight;
        break;
      case 9: digitOnePtr = nine;
        break;
      default: digitOnePtr = zero;
        break;
    }
    switch (secondScoreDigit) {
      case 0: digitTwoPtr = zero;
        break;
      case 1: digitTwoPtr = one;
        break;
      case 2: digitTwoPtr = two;
        break;
      case 3: digitTwoPtr = three;
        break;
      case 4: digitTwoPtr = four;
        break;
      case 5: digitTwoPtr = five;
        break;
      case 6: digitTwoPtr = six;
        break;
      case 7: digitTwoPtr = seven;
        break;
      case 8: digitTwoPtr = eight;
        break;
      case 9: digitTwoPtr = nine;
        break;
      default: digitTwoPtr = zero;
        break;
    }
    Serial.print("Score: ");
    Serial.print(score);
    Serial.print("\n");
    if (score > 9) {
      lc.clearDisplay(0);
      displayLetter(digitOnePtr, 5);
      displayLetter(digitTwoPtr, 1);
    } else {
      lc.clearDisplay(0);
      displayLetter(digitOnePtr, 2);
    }
  }
  /*
    randomLocation();
    while (!gamePaused && !gameOver) {
    getDirection();
    doMove(stickDirection);
    doCollision();
    updateBoard();

    Serial.print("Game Over: ");
    Serial.print(gameOver);
    Serial.print("\n");

    Serial.print("Snake Size: ");
    Serial.print(snakeSize);
    Serial.print("\n");
    Serial.print("Score: ");
    Serial.print(score);
    Serial.print("\n");
    /*
    Serial.print("Switch:  ");
    Serial.print(digitalRead(SW_pin));
    Serial.print("\n");
    Serial.print("X-axis: ");
    Serial.print(analogRead(X_pin));
    Serial.print("\n");
    Serial.print("Y-axis: ");
    Serial.println(analogRead(Y_pin));
    Serial.print("\n\n");

    //printBoard();
    }
  */
  /*
  Serial.print("Game State: ");
  Serial.print(gameState);
  Serial.print("\n");
  Serial.print("Paused: ");
  Serial.print(paused);
  Serial.print("\n");
  */
}

void loop() {
  //Serial.print(!digitalRead(SW_pin));
  if (!digitalRead(SW_pin)) {
    playGame();
  }
}
