// 2-------------------------------------------------------------- //
// --------------------Initial game settings---------------------- //
// --------------------------------------------------------------- //

//If you want to change the following data types during game, remove const in front
int initialSnakeLength = 3;

int snakeLength = initialSnakeLength;
int snakeSpeed = 20; //Speed will slow down as the number increase (miliseconds added to every snake movement)
int snakeDirection = 0; //It is 0 initially, when user presses a button, this has to change

// 2-------------------------------------------------------------- //
// ----end of----------Initial game settings---------------------- //
// --------------------------------------------------------------- //

// 3-------------------------------------------------------------- //
// ------Defining variables and creating matrix structure--------- //
// --------------------------------------------------------------- //

int gameboard[][] = {{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0}
}; //creating an empty matrix as game board

boolean win = false; 
boolean gameOver = false;

int snake_row=-1;
int snake_col=-1;
int food_row=-1;
int food_col=-1;

// -1 means that we don't want them in the board yet, we just wanted to create the variable
// corresponding functions will change the snake and food points

//direction constants
int up=1;
int right=2;
int down=3;
int left=4;

// 3-------------------------------------------------------------- //
// --end of--Defining variables and creating matrix structure----- //
// --------------------------------------------------------------- //


// 4-------------------------------------------------------------- //
// ------------------------- Setup code area, runs once   -------- //
// --------------------------------------------------------------- //


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
      return; //stops the food generator, orherwise it will run forever, because snake length is 64 and all pixels are occupied
    }
    //generate food to an empty led without generating on the snake
    
    food_col = int(random(8));
    food_row = int(random(8));

    while(gameboard[food_row][food_col] > 0){
      food_col = int(random(8));
      food_row = int(random(8));
    }
  }
}



void calculateSnake() {
    if(snakeDirection == up){
    snake_row--;
    fixEdge();
    //matrix.setLed(0, snake_row, snake_col, 1);
    }
    else if(snakeDirection == right){
    snake_col++;
    fixEdge();
    //matrix.setLed(0, snake_row, snake_col, 1);
    }
    else if(snakeDirection == down){
    snake_row++;
    fixEdge();
    //matrix.setLed(0, snake_row, snake_col, 1);
    }
    else if(snakeDirection == left){
    snake_col--;
    fixEdge();
    //matrix.setLed(0, snake_row, snake_col, 1);
    }
    else{
      return;
    }

  //if snake head collide with the body, game over
  if (999 > gameboard[snake_row][snake_col] && gameboard[snake_row][snake_col] > 1 && snakeDirection != 0) {
    gameOver = true;
    return;
  }

  //check if the food is eaten or not
  if (snake_row == food_row && snake_col == food_col) { //sadece yeni bir kafa ekle ve gövdeyi aynı bırak
    food_row = -1;
    food_col = -1; //resets food

    //increase snake length by 1 dot
    snakeLength ++;
    
    // add new snake head
    gameboard[snake_row][snake_col] = snakeLength;

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
        if (gameboard[row][col] == 0) print();//matrix.setLed(0, row, col, 0);
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
    
      //re initize game
      win=false;
      gameOver=false;
      snake_row = int(random(8));
      snake_col = int(random(8));
      food_row = -1;
      food_col = -1;
      snakeLength = initialSnakeLength;
      snakeDirection = 0;
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          if(gameboard[i][j] > 0){
             gameboard[i][j] = 0;
          }
        }
      }  
     
  }
  if (win){
    
      //re initize game
      win=false;
      gameOver=false;
      snake_row = int(random(8));
      snake_col = int(random(8));
      food_row = -1;
      food_col = -1;
      snakeLength = initialSnakeLength;
      snakeDirection = 0;
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          if(gameboard[i][j] > 0){
             gameboard[i][j] = 0;
          }
        }
      }  
  }
}

void draw_snake() {
  background(255);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if(gameboard[i][j] > 0){
        fill(255);
        rect(j*50,i*50,50,50);
      }else{
        fill(0);
        rect(j*50,i*50,50,50);
      }
    }
  }
}



// 5-------------------------------------------------------------- //
// --end of----------------- Functions -------------------------- //
// --------------------------------------------------------------- //

// 6-------------------------------------------------------------- //
// --------------------- Loop area, runs infinitely -------------- //
// --------------------------------------------------------------- //

void setup() {
  snake_row = int(random(8));
  snake_col = int(random(8));
  size(400,400);
}

void keyPressed() {
  int previousDirection=snakeDirection; //saving the last snake direction

    //determine and change the direction of the snake by reading the button presses
    if(key == 'w') snakeDirection = 1; // 1 is not pressed, 0 is pressed since we use PULLUP
    else if(key == 'd') snakeDirection = 2;
    else if(key == 's') snakeDirection = 3;
    else if(key == 'a') snakeDirection = 4;
    

    //igrone directional change by 180 degrees (not moving snake not affected)

    if (snakeDirection +2 == previousDirection && previousDirection !=0)
    snakeDirection=previousDirection;
    if (snakeDirection -2 == previousDirection && previousDirection !=0)
    snakeDirection=previousDirection;
}

void draw() {
  generateFood();
  if (millis() % 1000 < 500)
      gameboard[food_row][food_col] = 999;
  else
      gameboard[food_row][food_col] = 0;
  handleGameStates(); 
  calculateSnake();
  draw_snake();
  delay(200);
  
}



// 6-------------------------------------------------------------- //
// --end of------------- Loop area, runs infinitely -------------- //
// --------------------------------------------------------------- //
