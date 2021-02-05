 /* 
 * Copyright (c) 2018 Ondrej Telka. (https://ondrej.xyz/)
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "LedControl.h" // LedControl library is used for controlling a LED matrix. Find it using Library Manager or download zip here: https://github.com/wayoda/LedControl


// --------------------------------------------------------------- //
// ------------------------- user config ------------------------- //
// --------------------------------------------------------------- //

// there are defined all the pins, buttons for snake movement
const int button_UP = A0;   
const int button_RIGHT = A1;   
const int button_DOWN = A2;   
const int button_LEFT = A3;   

const int buzzer = 2; // to be connected to piezobuzzer positive pin

const int CLK = 9;   // clock for LED matrix
const int CS  = 8;  // chip-select for LED matrix
const int DIN = 7; // data-in for LED matrix

// LED matrix brightness: between 0(darkest) and 15(brightest)
const short intensity = 8;

// initial snake length (1...63, recommended 3)
const short initialSnakeLength = 3;


void setup() {
	initialize();         // initialize pins & LED matrix
  
  sound_device_initialize (); // plays the opening music
}

void loop() {
	generateFood();    // if there is no food, generate one
	scanButtons();    // watches button presses & blinks with food
	calculateSnake();  // calculates snake parameters
	handleGameStates();
}

// --------------------------------------------------------------- //
// -------------------- supporting variables --------------------- //
// --------------------------------------------------------------- //

LedControl matrix(DIN, CLK, CS, 1);

struct Point {
	int row, col;
	Point(int row = 0, int col = 0): row(row), col(col) {}
};

bool win = false;
bool gameOver = false;

// primary snake head coordinates (snake head), it will be randomly generated
Point snake(-1, -1);

// food is not anywhere yet
Point food(-1, -1);

// snake parameters
int snakeLength = initialSnakeLength; // choosed by the user in the config section
int snakeSpeed = 200; // Speed will slow down as the number increase (miliseconds added to every snake movement)
int snakeDirection = 0; // if it is 0, the snake does not move

// direction constants, tam olarak ne işe yarıyor?
const int up     = 1;
const int right  = 2;
const int down   = 3; // 'down - 2' must be 'up'
const int left   = 4; // 'left - 2' must be 'right'

// snake body segments storage
int gameboard[8][8] = {};




// --------------------------------------------------------------- //
// -------------------------- functions -------------------------- //
// --------------------------------------------------------------- //


// if there is no food, generate one, also check for victory
void generateFood() {
	if (food.row == -1 || food.col == -1) {
		// self-explanatory
		if (snakeLength >= 64) {
			win = true;
			return; // prevent the food generator from running, in this case it would run forever, because it will not be able to find a pixel without a snake
		}

		// generate food until it is in the right position
		do {
			food.col = random(8);
			food.row = random(8);
		} while (gameboard[food.row][food.col] > 0);
	}
}


// watches button presses & blinks with food
void scanButtons() {
	int previousDirection = snakeDirection; // save the last direction
	long timestamp = millis();  // Timestamp olmadan çalışıyor mu?

	while (millis() < timestamp + snakeSpeed) {

		// determine the direction of the snake
		if(digitalRead(button_UP) == false) snakeDirection = up;
		else if(digitalRead(button_DOWN) == false) snakeDirection = down;
		else if(digitalRead(button_LEFT) == false) snakeDirection = left;
		else if(digitalRead(button_RIGHT) == false) snakeDirection = right;

		// ignore directional change by 180 degrees (no effect for non-moving snake)
		snakeDirection + 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;
		snakeDirection - 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;

		// intelligently blink with the food
		matrix.setLed(0, food.row, food.col, millis() % 100 < 50 ? 1 : 0);
	}
}


// calculate snake movement data
void calculateSnake() {
	switch (snakeDirection) {
		case up:
			snake.row--;
			fixEdge();
			matrix.setLed(0, snake.row, snake.col, 1);
			break;

		case right:
			snake.col++;
			fixEdge();
			matrix.setLed(0, snake.row, snake.col, 1);
			break;

		case down:
			snake.row++;
			fixEdge();
			matrix.setLed(0, snake.row, snake.col, 1);
			break;

		case left:
			snake.col--;
			fixEdge();
			matrix.setLed(0, snake.row, snake.col, 1);
			break;

		default: // if the snake is not moving, exit
			return;
	}

	// if there is a snake body segment, this will cause the end of the game (snake must be moving) , bunun üzerine de çalışmak gerek
	if (gameboard[snake.row][snake.col] > 1 && snakeDirection != 0) {
		gameOver = true;
		return;
	}

	// check if the food was eaten
	if (snake.row == food.row && snake.col == food.col) {
		food.row = -1; // reset food
		food.col = -1;

		// increment snake length
		snakeLength++;

		// increment all the snake body segments, buraya da bakılacak
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (gameboard[row][col] > 0 ) {
					gameboard[row][col]++;
				}
			}
		}
	}

	// add new segment at the snake head location
	gameboard[snake.row][snake.col] = snakeLength + 1; // will be decremented in a moment

	// decrement all the snake body segments, if segment is 0, turn the corresponding led off
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			// if there is a body segment, decrement it's value
			if (gameboard[row][col] > 0 ) {
				gameboard[row][col]--;
			}

			// display the current pixel
			matrix.setLed(0, row, col, gameboard[row][col] == 0 ? 0 : 1);
		}
	}
}


// causes the snake to appear on the other side of the screen if it gets out of the edge
void fixEdge() {
	snake.col < 0 ? snake.col += 8 : 0;
	snake.col > 7 ? snake.col -= 8 : 0;
	snake.row < 0 ? snake.row += 8 : 0;
	snake.row > 7 ? snake.row -= 8 : 0;
}


void handleGameStates() {
	if (gameOver || win) {
    sound_game_over (); // plays game over music
		unrollSnake(); //bu fazlalık olabilir

		// re-init the game
		win = false;
		gameOver = false;
		snake.row = random(8);
		snake.col = random(8);
		food.row = -1;
		food.col = -1;
		snakeLength = initialSnakeLength;
		snakeDirection = 0;
		memset(gameboard, 0, sizeof(gameboard[0][0]) * 8 * 8); //bu satırı anlamamız lazım
		matrix.clearDisplay(0);
    sound_start_again ();
	}
}


void unrollSnake() {
	// switch off the food LED
	matrix.setLed(0, food.row, food.col, 0);

	delay(800);

	// flash the screen 5 times
	for (int i = 0; i < 5; i++) {
		// invert the screen
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				matrix.setLed(0, row, col, gameboard[row][col] == 0 ? 1 : 0);
			}
		}

		delay(20);

		// invert it back
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				matrix.setLed(0, row, col, gameboard[row][col] == 0 ? 0 : 1);
			}
		}

		delay(50);

	}


	delay(600);

	for (int i = 1; i <= snakeLength; i++) {
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (gameboard[row][col] == i) {
					matrix.setLed(0, row, col, 0);
					delay(100);
				}
			}
		}
	}
}


void initialize() {

  pinMode(button_UP, INPUT_PULLUP);
  pinMode(button_DOWN, INPUT_PULLUP);
  pinMode(button_LEFT, INPUT_PULLUP);
  pinMode(button_RIGHT, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
	matrix.shutdown(0, false);
	matrix.setIntensity(0, intensity);
	matrix.clearDisplay(0);

	randomSeed(analogRead(A5)); // Bu ne işe yarıyor ??  
	snake.row = random(8);
	snake.col = random(8);
}

// standard map function, but with floats
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void sound_device_initialize (){
 tone(buzzer,200,400);
 delay(200);
 tone(buzzer,400,400);
 delay(200);
 tone(buzzer,600,400);
 delay(200);
 tone(buzzer,800,400);
}

void sound_game_over (){
 tone(buzzer,800,400);
 delay(200);
 tone(buzzer,600,400);
 delay(200);
 tone(buzzer,400,400);
 delay(200);
 tone(buzzer,200,400);
}


void sound_start_again (){
 tone(buzzer,200,400);
 delay(200);
 tone(buzzer,800,400);
}
