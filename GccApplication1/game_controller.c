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

#define SPAWN_X 164
#define SPAWN_Y 90

//const uint8_t TFT_MAX_X = 131;
//const uint8_t TFT_MAX_Y = 175;
// node obstacle;	
entity obstacleList[3] = {{0, 0}};
entity player = {0,0};
	
int gameOver = 0;
int jumping = 0;

void InitGame(){
	gameOver = 0;
	jumping = 0;
	loadWorld();
	spawnPlayer();
	spawnObstacles();
}

/*
 *	Loads the world 
 */
void loadWorld() {
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

void spawnPlayer() {
	// player = { 20, 110 };
	player.x = 20;
	player.y = 90;
	// Player (Just a square for now)
	TFT_Window(player.x, player.y, player.x + 9, player.y + 9, TFT_Landscape);
	for(int i = 0; i < 100; i++) {
		SPISend8Bit(0x00);
	};
}

void spawnObstacles() {
	//obstacleList[3] = { 0, 0 };
	for (int i = 0; i < 3; i++)
	{
		obstacleList[i].x = SPAWN_X + (i * 50);
		obstacleList[i].y = SPAWN_Y;
		//entity newObst = { SPAWN_X + (50 + (i * 10)), SPAWN_Y };
		//TFT_Window(obstacleList[i].x, obstacleList[i].y, obstacleList[i].x + 10, obstacleList[i].y + 10, TFT_Landscape);
		//for(int i = 0; i < ; i++) {
			//SPISend8Bit(0xE0);
		//};
		//obstacleList[i] = newObst;
	}
}

void resetObstacle(entity *obst) {
	obst->x = SPAWN_X;
	obst->y = SPAWN_Y;
}

void moveObstacles() {
	for (int i = 0; i < 3; i++)
	{
		 //out of picture
		if (obstacleList[i].x > 170)
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
			resetObstacle(&obstacleList[i]);
		}
		obstacleList[i].x -= 1;
		if(checkCollision(obstacleList[i])) {
			// TODO end game
			// Buzzer on
			gameOver = 1;
			BUZZER_ON;
		};
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
	player.y = 75;
	if (counter > 30)
	{
		player.y = 90;
	}
	// draw player
	TFT_Window(player.x, player.y, player.x + 9, player.y + 9, TFT_Landscape);
	for(int i = 0; i < 100; i++) {
		SPISend8Bit(0x00);
	};
}

// Helper
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
	return (int) ((double) counter);
}