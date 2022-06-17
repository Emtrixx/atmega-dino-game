/*
 * game_controller.h
 *
 * Created: 06.06.2022 20:34:57
 *  Author: jesse
 */ 


#ifndef GAME_CONTROLLER_H_
#define GAME_CONTROLLER_H_

typedef struct {
	int x;
	int y;
}obstacle;

void InitGame();

void spawnObstacle();


#endif /* GAME_CONTROLLER_H_ */