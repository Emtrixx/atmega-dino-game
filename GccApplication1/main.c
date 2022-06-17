/*
 * GccApplication1.c
 *
 * Created: 01.06.2022 22:26:22
 * Author : jesse
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "tft_display.h"
#include "game_controller.h"


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

void init() {
	Display_init();
}

void Buttons_init() {
	//Button 1 als Input festlegen
	DDRD &= ~(1<<1);
	PORTD |= 1<<1;

	// Interrupt //
	sei();
	
	// Button1
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT17);
	
	// Timer0 A Match enable
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 250;
	// Configure CTC Mode
	TCCR0A |= (1<<WGM01);
	TCCR0A &= ~(1<<WGM00);
	TCCR0B &= ~(1<<WGM02);
	// Prescaler on 64
	TCCR0B |= 0b11;
	TCCR0B &= ~(1<<2);
}


// Button 1 ISR
ISR(PCINT0_vect) {
	// TODO Jump
}

ISR(TIMER0_COMPA_vect){
	// TODO measure ultrasonic time
	// jede ms
	// TODO gameticks
	// 
}