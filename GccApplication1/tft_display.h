/*
 * tft_display.h
 *
 * Created: 01.06.2022 22:35:48
 *  Author: jesse
 */ 


#ifndef TFT_DISPLAY_H_
#define TFT_DISPLAY_H_
#include <avr/io.h>

extern const uint8_t TFT_MAX_X;
extern const uint8_t TFT_MAX_Y;

enum TFT_Orientation {
	TFT_Portrait,
	TFT_Portrait180,
	TFT_Landscape,
	TFT_Landscape180
};

void Display_init();

void SendCommandSeq(const uint16_t * data, uint16_t Anzahl);

void SPISend8Bit(uint8_t data);

void TFT_Window(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, enum TFT_Orientation orientation);

#endif /* TFT_DISPLAY_H_ */