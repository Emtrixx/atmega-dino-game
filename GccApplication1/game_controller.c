/*
 * game_controller.c
 *
 * Created: 01.06.2022 23:00:15
 *  Author: jesse
 */
#include <avr/io.h>
#include "game_controller.h"
#include "tft_display.h"

//const uint8_t TFT_MAX_X = 131;
//const uint8_t TFT_MAX_Y = 175;
node obstacle;	


void InitGame(){
	// Background
	for(int i = 0; i < 23232; i++) {
		SPISend8Bit(0xFF);
	};
	// Floor
	TFT_Window(0, 100, 175, 110, TFT_Landscape);
	for(int i = 0; i < 23232; i++) {
		SPISend8Bit(0x00);
	};
	// Player (Just a square for now)
	TFT_Window(20, 110, 30, 100, TFT_Landscape);
	for(int i = 0; i < 23232; i++) {
		SPISend8Bit(0x00);
	};
	spawnObstacle();
}

/*
 *	Loads the world 
 */
void LoadWorld() {
	
}

void obstacles() {
	
}

void spawnObstacle() {
	obstacle new;
	new.x = 170;
	new.y = 110;
	TFT_Window(20, 110, 30, 100, TFT_Landscape);
	for(int i = 0; i < 23232; i++) {
		SPISend8Bit(0x00);
	};
}