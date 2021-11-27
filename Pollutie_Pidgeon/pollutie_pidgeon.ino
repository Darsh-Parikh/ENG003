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
#define GROUND_C1           GREEN
#define GROUND_C2           YELLOW
#define QUESTION_COLOR      CYAN

#define JUMP_BTN 2
#define OPTION_A 4
#define OPTION_B 7

#define MAX_Q 5
int q_i = 0;
char QUESTION_LIST[MAX_Q][5][100];

int fx, fy, fallRate, score, frame_counter, pillarPos, gapPos;
int highScore = 0;
bool jumped, restart;
bool running = true;
bool crashed = false;
long fps = 30000; // 20,000
bool correct_answer = false;

void setup() {
  // tft.reset();
  
  tft.begin(); 
  tft.setRotation(3); 

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor (45, 50);
  tft.setTextSize (3);
  tft.setTextColor(GREEN);
  tft.println("POLLUTIE BIRD !");
  tft.setCursor (55, 120);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("TEAM 4");

  delay(200);
  tft.fillScreen(BACKGROUND_COLOR);

  pinMode(JUMP_BTN, INPUT);
  pinMode(OPTION_A, INPUT);
  pinMode(OPTION_B, INPUT);
  
  make_questions();
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
    running = true;
  }
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
}

void drawPillar(int x, int gap) {
  int factory_height = 136 - gap;
  tft.fillRect(x+2, gap+92, 46, factory_height, PILLAR_COLOR);
}

void clearPillar(int x, int gap) {
  int factory_height = 140 - gap;
  tft.fillRect(x+45, gap+90, 5, factory_height, BACKGROUND_COLOR);
}

void drawFlappy(int x, int y) {
  // Upper & lower body
  tft.fillRect(x, y, 30, 20, GREY);
  // // Eye
  tft.fillRect(x+16, y+7, 8, 8, WHITE);
  // // Beak
  
}

void clearFlappy(int x, int y) {
 tft.fillRect(x, y, 34, 24, BACKGROUND_COLOR); 
}

void checkCollision() {
  // Collision with ground
  if (fy > 206 || fy < 0) { crashed = true; }
  
  // Collision with pillar
  if (fx + 34 > pillarPos && fx < pillarPos + 50)
    if (fy + 24 > gapPos + 90)
      crashed = true;
  
  if (crashed) {
    question_time();
    crashed = false;

    if (!correct_answer) {
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(75, 75);
      tft.print("Game Over!");
      tft.setCursor(75, 125);
      tft.print("Score:");
      tft.setCursor(220, 125);
      tft.print(score);
      
      // stop animation
      running = false;
      
      // delay to stop any last minute clicks from restarting immediately
      delay(1000);
    }
  }
}

void drawLoop() {
  // clear moving items
  
  tft.fillRect(pillarPos+45, gapPos+90, 5, 140-gapPos, BACKGROUND_COLOR);
  tft.fillRect(fx, fy, 34, 24, BACKGROUND_COLOR);

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
  tft.fillRect(pillarPos+2, gapPos+92, 5, 136-gapPos, PILLAR_COLOR);
  drawFlappy(fx, fy);
}

// void add_question(char* question[], char* opA[], char* opB[], char* correct[]) {
//   if (next_q == MAX_Q) {} // handle error ?? Or ignore?
//   else {
//     char new_q[] = {question, opA, opB, correct};
//     // QUESTION_MAP* new_q = {
//     //   {'q', question},
//     //   {'a', opA},
//     //   {'b', opB},
//     //   {'c', correct}
//     // }
//     QUESTION_LIST[next_q] = new_q;
//     extern next_q++;
//   }
// }

void boundedText(int x, int y, char* text) {
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

int add_question(char q[], char a[], char b[], char c[], char e[]) {
  if (q_i == MAX_Q-1) { return 1; }
  
  char curr;
  int i;
  
  for (i = 0; (curr = q[i]) != '\0'; ++i) { QUESTION_LIST[q_i][0][i] = curr; }
  QUESTION_LIST[q_i][0][i] = curr;
  
  for (i = 0; (curr = a[i]) != '\0'; ++i) { QUESTION_LIST[q_i][1][i] = curr; }
  QUESTION_LIST[q_i][1][i] = curr;
  
  for (i = 0; (curr = b[i]) != '\0'; ++i) { QUESTION_LIST[q_i][2][i] = curr; }
  QUESTION_LIST[q_i][2][i] = curr;
  
  for (i = 0; (curr = c[i]) != '\0'; ++i) { QUESTION_LIST[q_i][3][i] = curr; }
  QUESTION_LIST[q_i][3][i] = curr;
  
  for (i = 0; (curr = e[i]) != '\0'; ++i) { QUESTION_LIST[q_i][4][i] = curr; }
  QUESTION_LIST[q_i][4][i] = curr;
  
  ++q_i;
  return 0;
}

void make_questions() {
  // Only enter the number of questions specificed by MAX_Q
  // Enter in order: Question, Option A, Option B, Correct Option, Explanation
    add_question(
      "Some pretty big Question here...probably much bigger than this one...?",
      "Some specific option A here...probably much bigger than this one...",
      "Some specific option B here...probably much bigger than this one...",
      "A", 
      "Some Explanation here..."
    );
}

void question_time() {
  // Create a spot to ask question... Enter a loop
  int curr_q = 0; // Make this random between 0 and q_i
  
  //! ERROR HERE
  char question[] = QUESTION_LIST[curr_q][0];
  char optionA[] =  QUESTION_LIST[curr_q][1];
  char optionB[] =  QUESTION_LIST[curr_q][2];

  tft.fillRect(10, 10, 300, 220, QUESTION_COLOR);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);

  boundedText(15, 15, question);
  boundedText(15, 70, optionA);
  boundedText(15, 110, optionB);

  bool A = false;
  bool B = false;
  char selection = 'Z';
  // char correct_option = curr_Q[3];
  char correct_option = (char)QUESTION_LIST[curr_q][3];

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

    char correct_text[] = "YAY!! That was the CORRECT answer. Good Job";
    char restart_text[] = "Your game will restart in a little bit";
    boundedText(25, 160, correct_text);
    boundedText(25, 200, restart_text);
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
    char incorrect_text[] = "That answer was INCORRECT!";
    boundedText(10, 160, incorrect_text);
    tft.setTextColor(BLACK);
    
    // if (correct_option == 'A') { tft.drawCircle(30, 80, 10, GREEN); }
    // else { tft.drawCircle(30, 130, 10, GREEN); }
    
    delay(2000);
    tft.fillRect(10, 10, 300, 220, QUESTION_COLOR);
    
    tft.setCursor(40, 10);
    tft.setTextSize(3);
    tft.print("EXPLANATION");
    tft.setTextSize(2);
    
    char explanation[] = QUESTION_LIST[curr_q][4];
    boundedText(10, 40, explanation);
    
    delay(5000);
    tft.fillScreen(BACKGROUND_COLOR);
  }
}
