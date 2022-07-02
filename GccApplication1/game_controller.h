/*
 * game_controller.h
 *
 * Created: 06.06.2022 20:34:57
 *  Author: jesse
 */ 


#ifndef GAME_CONTROLLER_H_
#define GAME_CONTROLLER_H_

#define BUZZER_ON		PORTC |= 1<<PORTC1
#define BUZZER_OFF	PORTC &= ~(1<<PORTC1)

typedef struct {
	int x;
	int y;
} entity;

int gameOver;
int jumping;
int speed;
//static volatile uint8_t triggerOnBool = 0;

void InitGame();
void startGame();

void moveObstacles();

void jump(int counter);

#endif /* GAME_CONTROLLER_H_ */