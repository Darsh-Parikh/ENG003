//#include <EEPROM.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Screen size: 320 x 240

#define BLACK       0x0000
#define BLUE        0x001F
#define DARKBLUE    0x0010
#define VIOLET      0x8888
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define GREY        0x8410
#define GOLD        0xFEA0
#define BROWN       0xA145
#define SILVER      0xC618
#define LIME        0x07E0

#define TFT_DC   9
#define TFT_CS   10
#define TFT_RST  8
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Additions + Changeable stuff
#define BACKGROUND_COLOR    BLUE
#define PILLAR_COLOR        BLACK
#define PILLAR_BOUNDARY     BLACK
#define SMOKE_COLOR         BROWN
#define GROUND_C1           GREEN
#define GROUND_C2           YELLOW
#define QUESTION_COLOR      CYAN

#define JUMP_BTN 2
#define OPTION_A 4
#define OPTION_B 7

#define MAX_Q 4
#define MAX_Q_C 51
#define MAX_E_C 76
#define MAX_O_C 21
int q_i = 0;
char QUESTION_LIST[MAX_Q][MAX_Q_C];
char OPTIONS_LIST[MAX_Q][2][MAX_O_C];
char CORRECT_LIST[MAX_Q];
char EXPLANATIONS_LIST[MAX_Q][MAX_E_C];

int fx, fy, fallRate, score, frame_counter, pillarPos, gapPos;
int highScore = 0;
bool jumped, restart;
bool running = true;
bool crashed = false;
long fps = 30000; // 20,000
bool correct_answer = false;

void setup() {
  pinMode(JUMP_BTN, INPUT);
  pinMode(OPTION_A, INPUT);
  pinMode(OPTION_B, INPUT);
  // tft.reset();
  tft.begin(); 
  tft.setRotation(3); 

  tft.fillScreen(BLACK);
  tft.setCursor (15, 50);
  tft.setTextSize (3);
  tft.setTextColor(GREEN);
  tft.println("POLLUTIE PIDGEON");
  tft.setCursor (120, 120);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("TEAM 4");

  delay(1000); 
  make_questions();
  wait_for_press();
  
  tft.fillScreen(BACKGROUND_COLOR);
  
  startGame();
  frame_counter = 0;
}

void loop(void) {
  while (running) {
    jumped = digitalRead(JUMP_BTN);
  
    if (jumped) {
      // fly up
      fallRate = -6; 
    }

    if (frame_counter >= fps) {
      drawLoop();
      checkCollision();
      if (correct_answer) { 
        drawLoop();
        correct_answer = false;
      }
      frame_counter = 0;
    }
    else { frame_counter += 5; }
  }
  
  restart = digitalRead(JUMP_BTN);
  if (restart) {
    startGame();
    running = true;
  }
}

void wait_for_press() {
  bool pressed;
  do {
    pressed = digitalRead(JUMP_BTN);
  }
  while (!pressed);
}

void startGame() {
  fx = 5;
  fy = 10;
  fallRate = 1;
  pillarPos = 320;
  gapPos = 60;
  crashed = false;
  score = 0;
  
  tft.fillScreen(BLUE);
  tft.fillRect(0, 230, 320, 10, GROUND_C2);
  tft.fillRect(0, 0, 320, 2, SMOKE_COLOR);
}

void drawPillar(int x, int gap) {
  int factory_height = 136 - gap;
  int smoke_lowPoint = gap - 4;
  tft.fillRect(x+2, gap+92, 5, factory_height, PILLAR_COLOR);
  
  tft.fillRect(x+2, 2, 5, smoke_lowPoint, SMOKE_COLOR);
}

void clearPillar(int x, int gap) {
  int factory_height = 140 - gap;
  tft.fillRect(x+45, gap+90, 5, factory_height, BACKGROUND_COLOR);
  tft.fillRect(x+45, 2, 5, gap, BACKGROUND_COLOR);
}

void drawFlappy(int x, int y) {
  // Upper & lower body
  tft.fillRect(x, y, 30, 20, GREY);
  // // Eye
  tft.fillRect(x+16, y+7, 8, 8, WHITE);
}

void clearFlappy(int x, int y) {
 tft.fillRect(x, y, 34, 24, BACKGROUND_COLOR); 
}

void checkCollision() {
  // Collision with ground
  if (fy > 206 || fy < 0) { crashed = true; }
  
  // Collision with pillar
  if (fx + 34 > pillarPos && fx < pillarPos + 50)
    if (fy < gapPos || fy + 24 > gapPos + 90)
      crashed = true;
  
  if (crashed) {
    question_time();
    crashed = false;

    if (!correct_answer) {
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(75, 50);
      tft.print("Game Over!");
      tft.setCursor(75, 100);
      tft.print("Score:");
      tft.setCursor(220, 100);
      tft.print(score);
      
      // stop animation
      running = false;
      
      tft.setCursor(30, 150);
      tft.print("Press Jump to Restart");
      
      // delay to stop any last minute clicks from restarting immediately
      delay(1000);
    }
  }
}

void drawLoop() {
  // clear moving items
  clearPillar(pillarPos, gapPos);
  clearFlappy(fx, fy);  

  // move items
  fy += fallRate;
  fallRate++;

  pillarPos -= 5;
  if (pillarPos == 0) {
    score += 10;
  }
  else if (pillarPos < -50) {
    pillarPos = 320;
    gapPos = random(20, 120);
  }

  // draw moving items & animate
  drawPillar(pillarPos, gapPos);
  drawFlappy(fx, fy);
}

void boundedText(int x, int y, const char* text) {
 int max_lines = 3;
 int max_c = 25;
 int dim = 15;
 
 char all_text[max_lines][max_c];
 char line[max_c];
 int at_i, l_i, i;
 at_i = l_i = i = 0;
 char c;
 while ((c = text[i]) != '\0') {
   line[l_i] = c;
   ++i;
   ++l_i;
   if (l_i >= max_c-1) {
     line[l_i] = '\0';
     for (int k = 0; k < 25; ++k) { all_text[at_i][k] = line[k]; }
     l_i = 0;
     ++at_i;
     if (at_i == max_lines) { break; }
   }
 }
 line[l_i] = '\0';
 for (int k = 0; k < 25; ++k) { all_text[at_i][k] = line[k]; }
 ++at_i;
  
 for (int j = 0; j < at_i; ++j) {
   tft.setCursor(x, y + (dim*j));
   tft.print(all_text[j]);
 }
}

int add_question(const char q[], const char a[], const char b[], const char c, const char e[]) {
  if (q_i == MAX_Q-1) { return 1; }
  
  char curr;
  int i;
  
  for (i = 0; (curr = q[i]) != '\0' && i < MAX_Q_C; ++i) { QUESTION_LIST[q_i][i] = curr; }
  QUESTION_LIST[q_i][i] = curr;
  
  for (i = 0; (curr = a[i]) != '\0' && i < MAX_O_C; ++i) { OPTIONS_LIST[q_i][0][i] = curr; }
  OPTIONS_LIST[q_i][0][i] = curr;
  
  for (i = 0; (curr = b[i]) != '\0' && i < MAX_O_C; ++i) { OPTIONS_LIST[q_i][1][i] = curr; }
  OPTIONS_LIST[q_i][1][i] = curr;
  
  CORRECT_LIST[q_i] = c;
  
  for (i = 0; (curr = e[i]) != '\0' && i < MAX_E_C; ++i) { EXPLANATIONS_LIST[q_i][i] = curr; }
  EXPLANATIONS_LIST[q_i][i] = curr;
  
  ++q_i;
  return 0;
}

void make_questions() {
  // Only enter the number of questions specificed by MAX_Q
  // Enter in order: Question, Option A, Option B, Correct Option, Explanation
    add_question(
      "Smoke and Pollution causes breathing trouble?",
      "A) True",
      "B) False",
      'A', 
      "Smoke has harmful content, and breathing it can make us sick"
    );
    add_question(
      "Factories with smoke are bad for nature?",
      "A) YES",
      "B) NO",
      'A',
      "Smoke makes it harder for animals to breathe, killing them"  
    );
    add_question(
      "Cars or Bicycles? (For short distances)",
      "A) Cars",
      "B) Bicycles",
      'B',
      "Cars pollute. Bicycles don't, so good for short distances"
    );
}

void question_time() {
  // Create a spot to ask question... Enter a loop
  int curr_q = random(0, q_i-1);
  char question[MAX_Q_C], optionA[MAX_O_C], optionB[MAX_O_C], correct_option, explanation[MAX_E_C];
  
  int z;
  char ch;
  for (z = 0; (ch = QUESTION_LIST[curr_q][z]) != '\0' && z < MAX_Q_C-1; ++z) { question[z] = ch; }
  question[z] = '\0';
  for (z = 0; (ch = OPTIONS_LIST[curr_q][0][z]) != '\0' && z < MAX_O_C-1; ++z) { optionA[z] = ch; }
  optionA[z] = '\0';
  for (z = 0; (ch = OPTIONS_LIST[curr_q][1][z]) != '\0' && z < MAX_O_C-1; ++z) { optionB[z] = ch; }
  optionB[z] = '\0';
  correct_option = CORRECT_LIST[curr_q];
  for (z = 0; (ch = EXPLANATIONS_LIST[curr_q][z]) != '\0' && z < MAX_E_C-1; ++z) { explanation[z] = ch; }
  explanation[z] = '\0';

  tft.fillRect(10, 10, 300, 220, QUESTION_COLOR);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);

  boundedText(15, 15, question);
  boundedText(15, 70, optionA);
  boundedText(15, 110, optionB);

  bool A = false;
  bool B = false;
  char selection = 'Z';

  // Wait for button response
  while (true) {
    A = digitalRead(OPTION_A);
    B = digitalRead(OPTION_B);

    if (A || B) {
      if (A) { selection = 'A'; }
      else { selection = 'B'; }
      break;
    }
  }
  
  // Check answer...
  if (selection == correct_option) {
    correct_answer = true;
    
    boundedText(25, 160, "YAY! CORRECT, Good Job");
    delay(2000);

    tft.fillRect(0, 0, 320, 230, BACKGROUND_COLOR);
    tft.fillRect(0, 230, 320, 10, GROUND_C2);
    fx = 5;
    fy = 10;
    pillarPos = 260;
    fallRate = 1;
  }
  else {
    correct_answer = false;

    tft.setTextColor(RED);
    boundedText(10, 160, "INCORRECT!");
    tft.setTextColor(BLACK);
    
    // if (correct_option == 'A') { tft.drawCircle(30, 80, 10, GREEN); }
    // else { tft.drawCircle(30, 130, 10, GREEN); }
    
    delay(2000);
    tft.fillRect(10, 10, 300, 220, QUESTION_COLOR);
    
    tft.setCursor(50, 15);
    tft.setTextSize(3);
    tft.print("EXPLANATION");
    tft.setTextSize(2);

    boundedText(15, 50, explanation);
    
    delay(5000);
    boundedText(15, 180, "Press Jump to proceed");
    wait_for_press();
    delay(100);    
    tft.fillScreen(BACKGROUND_COLOR);
  }
}
