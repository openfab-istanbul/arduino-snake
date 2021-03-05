// 1-------------------------------------------------------------- //
// -----------Wiring of buttons, Buzzer and LED Matrix------------ //
// --------------------------------------------------------------- //

// Buttons' other pin should be connected to ground
const int Button_Up = 7;
const int Button_Down = 5;
const int Button_Left = 6;
const int Button_Right = 8;

const int Buzzer = 9; //to be connected to piezoBuzzer positive pin

const int Matrix_CLK = 4; // CLK pin of the LED Matrix
const int Matrix_CS = 3;  // CS pin of the LED Matrix
const int Matrix_DIN = 2;  // DIN pin of the LED Matrix

// 1-------------------------------------------------------------- //
// ---end of--Wiring of buttons, Buzzer and LED Matrix------------ //
// --------------------------------------------------------------- //

// 2-------------------------------------------------------------- //
// --------------------Initial game settings---------------------- //
// --------------------------------------------------------------- //

//If you want to change the following data types during game, remove const in front
const int Matrix_Brightness = 8;
const int initialSnakeLength = 3;

int snakeLength = initialSnakeLength;
int snakeSpeed = 200; //Speed will slow down as the number increase (miliseconds added to every snake movement)
int snakeDirection = 0; //It is 0 initially, when user presses a button, this has to change

// 2-------------------------------------------------------------- //
// ----end of----------Initial game settings---------------------- //
// --------------------------------------------------------------- //

// 3-------------------------------------------------------------- //
// ------Defining variables and creating matrix structure--------- //
// --------------------------------------------------------------- //

#include "LedControl.h" // LED Matrix Library

LedControl matrix(Matrix_DIN,Matrix_CLK,Matrix_CS,1);

int gameboard[8][8] = {}; //creating an empty matrix as game board

bool win = false; 
bool gameOver = false;

int snake_row=-1;
int snake_col=-1;
int food_row=-1;
int food_col=-1;

// -1 means that we don't want them in the board yet, we just wanted to create the variable
// corresponding functions will change the snake and food points

//direction constants
const int up=1;
const int right=2;
const int down=3;
const int left=4;

// 3-------------------------------------------------------------- //
// --end of--Defining variables and creating matrix structure----- //
// --------------------------------------------------------------- //


// 4-------------------------------------------------------------- //
// ------------------------- Setup code area, runs once   -------- //
// --------------------------------------------------------------- //
void setup() {
  // put your setup code here, to run once
  initialize(); // initialize pins & LED matrix
  sound_device_initialize (); // plays the opening music

  
}

// 4-------------------------------------------------------------- //
// ---end of---------------- Setup code area, runs once   -------- //
// --------------------------------------------------------------- //


// 5-------------------------------------------------------------- //
// -------------------------- Functions -------------------------- //
// --------------------------------------------------------------- //

void generateFood () {
  if (food_row == -1 || food_col == -1) {
    if (snakeLength >= 64) {
      win=true;
      return; //stops the food generator, otherwise it will run forever, because snake length is 64 and all pixels are occupied
    }
    //generate food to an empty led without generating on the snake

    while(gameboard[food_row][food_col] > 0){
      food_col = random(8);
      food_row = random(8);
    }
  }
}

void scanButtons(){
  int previousDirection=snakeDirection; //saving the last snake direction
  long timestamp = millis(); // creating a timestamp variable for the game speed

  while (millis()<timestamp + snakeSpeed) {

    //determine and change the direction of the snake by reading the button presses
    if(digitalRead(Button_Up) == false) snakeDirection= up; // 1 is not pressed, 0 is pressed since we use PULLUP
    else if(digitalRead(Button_Right) == false) snakeDirection= right;
    else if(digitalRead(Button_Down) == false) snakeDirection= down;
    else if(digitalRead(Button_Left) == false) snakeDirection= left;
    

    //igrone directional change by 180 degrees (not moving snake not affected)

    if (snakeDirection +2 == previousDirection && previousDirection !=0)
    snakeDirection=previousDirection;
    if (snakeDirection -2 == previousDirection && previousDirection !=0)
    snakeDirection=previousDirection;

    

    // food blinking effect (this code is here because this is the only loop that runs continuously)
    if(millis() % 100 < 50)
    matrix.setLed(0, food_row, food_col, 1);
    else
    matrix.setLed(0, food_row, food_col, 0);
    

  }
}

void calculateSnake() {
  switch (snakeDirection) {
    case up:
    snake_row--;
    fixEdge();
    matrix.setLed(0, snake_row, snake_col, 1);
    break;  

    case right:
    snake_col++;
    fixEdge();
    matrix.setLed(0, snake_row, snake_col, 1);
    break;  

    case down:
    snake_row++;
    fixEdge();
    matrix.setLed(0, snake_row, snake_col, 1);
    break;  

    case left:
    snake_col--;
    fixEdge();
    matrix.setLed(0, snake_row, snake_col, 1);
    break; 

    default: //if snake is not moving, go to the starting line of the loop
    return;
  }

  //if snake head collide with the body, game over
  if (gameboard[snake_row][snake_col] > 1 && snakeDirection != 0) {
    gameOver = true ;
    return;
  }

  //check if the food is eaten or not
  if (snake_row == food_row && snake_col == food_col) { //sadece yeni bir kafa ekle ve gövdeyi aynı bırak
    digitalWrite(LED_2,HIGH);
    food_row = -1;
    food_col = -1; //resets food

    //increase snake length by 1 dot
    snakeLength ++;
    
    // add new snake head
    gameboard[snake_row][snake_col] = snakeLength;
    delay(100);
    digitalWrite(LED_2,LOW);

  }
  else{ // bütün sayıları bir sıra kaydır

    // add new snake head
    gameboard[snake_row][snake_col] = snakeLength +1; //

    //reduce previous numbers  
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        //reduce previous numbers
        if (gameboard[row][col] > 0 ) gameboard[row][col]--;

        //display the current pixel
        if (gameboard[row][col] == 0) matrix.setLed(0, row, col, 0);
      }
           
    }
  }
}

//to be able to make the snake pass through walls
void fixEdge() {

  if (snake_col < 0)
  snake_col += 8;
  if (snake_col > 7)
  snake_col -= 8;
  if (snake_row < 0)
  snake_row += 8;
  if (snake_row > 7)
  snake_row -= 8;


}

void handleGameStates() {
  if (gameOver){
      digitalWrite(LED_1,HIGH);
      sound_game_over (); // plays game over music 
      delay(500);
      digitalWrite(LED_1,LOW);    
      //re initize game
      win=false;
      gameOver=false;
      snake_row = random(8);
      snake_col = random(8);
      food_row = -1;
      food_col = -1;
      snakeLength = initialSnakeLength;
      snakeDirection = 0;
      memset(gameboard, 0 , sizeof(gameboard));
      matrix.clearDisplay(0);
      sound_start_again;    
     
  }
  if (win){
      sound_win (); // plays win music 
    
      //re initize game
      win=false;
      gameOver=false;
      snake_row = random(8);
      snake_col = random(8);
      food_row = -1;
      food_col = -1;
      snakeLength = initialSnakeLength;
      snakeDirection = 0;
      memset(gameboard, 0 , sizeof(gameboard));
      matrix.clearDisplay(0);
      sound_start_again;    
  }
}

void initialize() {

  pinMode(Button_Up, INPUT_PULLUP);
  pinMode(Button_Down, INPUT_PULLUP);
  pinMode(Button_Left, INPUT_PULLUP);
  pinMode(Button_Right, INPUT_PULLUP);

  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
  
  matrix.shutdown(0, false);
  matrix.setIntensity(0, Matrix_Brightness);
  matrix.clearDisplay(0);

  randomSeed(analogRead(A5)); 
  // Since analog inputs are not stable when not connected,
  // it is a good source for random number generation
  snake_row = random(8);
  snake_col = random(8);
}

void sound_win (){
 tone(Buzzer,200,400);
 delay(200);
 tone(Buzzer,800,400);
 delay(200);
 tone(Buzzer,300,400);
 delay(200);
 tone(Buzzer,600,400);
}

void sound_device_initialize (){
 tone(Buzzer,200,400);
 delay(200);
 tone(Buzzer,400,400);
 delay(200);
 tone(Buzzer,600,400);
 delay(200);
 tone(Buzzer,800,400);
}

void sound_game_over (){
 tone(Buzzer,800,400);
 delay(200);
 tone(Buzzer,600,400);
 delay(200);
 tone(Buzzer,400,400);
 delay(200);
 tone(Buzzer,200,400);
}

void sound_start_again (){
 tone(Buzzer,200,400);
 delay(200);
 tone(Buzzer,800,400);
}

// 5-------------------------------------------------------------- //
// --end of----------------- Functions -------------------------- //
// --------------------------------------------------------------- //

// 6-------------------------------------------------------------- //
// --------------------- Loop area, runs infinitely -------------- //
// --------------------------------------------------------------- //

void loop() {
  // put your main code here, to run repeatedly:
  generateFood();    // if there is no food, generate one
  scanButtons();    // watches button presses & blinks with food
  calculateSnake();  // calculates snake parameters
  //handleGameStates();
}

// 6-------------------------------------------------------------- //
// --end of------------- Loop area, runs infinitely -------------- //
// --------------------------------------------------------------- //
