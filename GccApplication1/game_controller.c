/*
 * game_controller.c
 *
 * Created: 01.06.2022 23:00:15
 *  Author: jesse
 */
#include <avr/io.h>
#include "game_controller.h"
#include "tft_display.h"
#include <math.h>
#include <stdlib.h>

#define SPAWN_X 164
#define SPAWN_Y 90
#define PI 3.14

//#define TRIGGER_ON PORTD |= (1<<PORTD0)
//#define TIMER2_ON	TCCR2B |= (1<<CS21)

//const uint8_t TFT_MAX_X = 131;
//const uint8_t TFT_MAX_Y = 175;
// node obstacle;	
entity obstacleList[3] = {{0, 0}};
entity player = {0,0};
	
int gameOver;
int jumping;

static volatile uint16_t scoreCounter = 0;

void loadWorld(void);
void spawnPlayer(void);
void spawnObstacles(void);
int checkCollision(entity obst);

void InitGame(){
	gameOver = 0;
	jumping = 0;
	speed = 0;
	loadWorld();
	spawnPlayer();
	spawnObstacles();
	counterDisplay();
	randInit();
}

/*
 *	Loads the world 
 */
void loadWorld(void) {
	// Background
	TFT_Window(0, 0, 175, 131, TFT_Landscape);
	for(int i = 0; i < 23232; i++) {
		SPISend8Bit(0xFF);
	};
	// Floor
	TFT_Window(0, 100, 175, 109, TFT_Landscape);
	for(int i = 0; i < 1760; i++) {
		SPISend8Bit(0x00);
	};
}

void randInit() {
   //time_t t;
   //
   //srand((unsigned) time(&t));
}

void spawnPlayer(void) {
	// player = { 20, 110 };
	player.x = 20;
	player.y = 90;
	// Player (Just a square for now)
	TFT_Window(player.x, player.y, player.x + 9, player.y + 9, TFT_Landscape);
	for(int i = 0; i < 100; i++) {
		SPISend8Bit(0x00);
	};
}

void spawnObstacles(void) {
	for (int i = 0; i < 3; i++)
	{
		obstacleList[i].x = SPAWN_X + (i * 50);
		obstacleList[i].y = SPAWN_Y;
	}
}

void resetObstacle(entity *obst) {
	obst->x = SPAWN_X + (rand()%15);
	obst->y = SPAWN_Y;
}

void moveObstacles() {
	for (int i = 0; i < 3; i++)
	{
		 //out of picture
		if (obstacleList[i].x > 175)
		{
			obstacleList[i].x -= 1;
			continue;
		}
		//clear
		TFT_Window(obstacleList[i].x, obstacleList[i].y, obstacleList[i].x + 9, obstacleList[i].y + 9, TFT_Landscape);
		for(int i = 0; i < 100; i++) {
			SPISend8Bit(0xFF);
		};
		 //reset if passed
		if (obstacleList[i].x == 0)
		{
			counterDisplay();
			resetObstacle(&obstacleList[i]);
		}
		obstacleList[i].x -= 1;
		//check Collision
		if(checkCollision(obstacleList[i])) {
			BUZZER_ON;
			setGameOver();
			return;
		};
		// draw obstacle
		TFT_Window(obstacleList[i].x, obstacleList[i].y, obstacleList[i].x + 9, obstacleList[i].y + 9, TFT_Landscape);
		for(int i = 0; i < 100; i++) {
			SPISend8Bit(0xE0);
		};
	}
}

int checkCollision(entity obst) {
	if (obst.x < (player.x + 10) && (obst.x + 10) > player.x)
	{
		if (obst.y < (player.y + 10))
		{
			return 1;
		}
	}
	return 0;
}

// TODO implement parabolic jump
void jump(int counter) {
	// clear player
	TFT_Window(player.x, player.y, player.x + 9, player.y + 9, TFT_Landscape);
	for(int i = 0; i < 100; i++) {
		SPISend8Bit(0xFF);
	};
	// set jump 
	player.y = jumpHeight2(counter);
	// draw player
	TFT_Window(player.x, player.y, player.x + 9, player.y + 9, TFT_Landscape);
	for(int i = 0; i < 100; i++) {
		SPISend8Bit(0x00);
	};
}

void counterDisplay() {
	speed = (int) (scoreCounter / 3);
	char str[12];
	sprintf(str, "%d", scoreCounter);
	TFT_Print(str, 85, 12, 2, TFT_8BitBlack, TFT_8BitWhite, TFT_Landscape);
	scoreCounter++;
}

void setGameOver() {
	TFT_Window(0, 0, 175, 131, TFT_Landscape);
	for(int i = 0; i < 23232; i++) {
		SPISend8Bit(0xFF);
	};
	gameOver = 1;
	scoreCounter = 0;
	char str[] = "Game Over";
	TFT_Print(str, 35, 12, 2, TFT_8BitBlack, TFT_8BitWhite, TFT_Landscape);
}

//void measureDistance() {
	//triggerOnBool = 1;
	//TCNT2 = 0;
	//TRIGGER_ON;
	//TIMER2_ON;
//}

// Helper
int jumpHeight(int counter) {
	static volatile float acc = 0.0;
	static volatile float denom = 2.0;
	int height;
	
	if (counter<10) {
		acc += ((float)1)/denom;
		denom++;
		height = 90 - ((int) (15.0 * acc));
		logFloat(acc);
	}else if (counter < 20) {
		acc = 0.0;
		denom = 2.0;
		height = 75;
	} else if (counter < 30) {
		acc += ((float)1)/denom;
		height = 75 + ((int) (15.0 * acc));
	} else {
		acc = 0.0;
		denom = 2.0;
		height = 90;
	}
	return height;
}

int jumpHeight2(int counter) {
	static volatile int acc = 1;
	int height;
	
	if (counter<10) {
		height = 90 - sinJump(acc);
		acc++;
	}else if (counter < 26) {
		acc = 1;
		height = 70;
	} else if (counter < 36) {
		height = 70 + sinJump(acc);
		acc++;
	} else {
		acc = 1;
		height = 90;
	}
	return height;
}

void logFloat(float counter) {
	TFT_Window(85, 42, 170, 60, TFT_Landscape);
	for(int i = 0; i < 1643; i++) {
		SPISend8Bit(0xFF);
	};
	char str[12];
	sprintf(str, "%d", counter);
	TFT_Print(str, 85, 62, 1, TFT_8BitBlack, TFT_8BitWhite, TFT_Landscape);
}

void logInt2(uint16_t counter) {
	TFT_Window(85, 42, 170, 60, TFT_Landscape);
	for(int i = 0; i < 1643; i++) {
		SPISend8Bit(0xFF);
	};
	char str[12];
	sprintf(str, "%d", counter);
	TFT_Print(str, 85, 42, 2, TFT_8BitBlack, TFT_8BitWhite, TFT_Landscape);
}

int pow_int(int n, int power) {
	if (power == 0)
	{
		return 1;
	}
	int sum = 0;
	for(int i = 0; i < power; i++) {
		sum += n;
	}
	return sum;
}

int sinJump(int counter) {
	double n = (double)counter / (10.0 / (PI/2));
	return 20.0 * sin(n);
}