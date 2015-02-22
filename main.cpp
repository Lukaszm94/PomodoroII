/*
 * main.cpp
 *
 *  Created on: 29 paü 2014
 *      Author: Lukasz
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pin.hpp"
#include "portb.hpp"
#include "portc.hpp"
#include "portd.hpp"
#include "LedBar.h"
#include "PomodoroTimer.h"
#include "InterruptManager.h"


volatile PomodoroTimer pomodoroTimer;
volatile InterruptManager interruptManager;





int main (void)
{
	TCCR1B = (1<<WGM12) | (1<<CS11);
	TIMSK |= (1<<OCIE1A);
	OCR1A = 126;
	sei();

	while(1)
	{
	}

}

ISR(TIMER1_COMPA_vect) { //this is called every 0.25ms
	interruptManager.update();
}
