/*
 * Buzzer.h
 *
 *  Created on: 9 lis 2014
 *      Author: Lukasz
 */

#ifndef BUZZER_H_
#define BUZZER_H_
#include "pin.hpp"
#include "portc.hpp"

const uint16_t beepsTableSize = 10;
const uint16_t beepIntervalMs = 400; //time of silence between beeps

template <typename Port, int Pin>
class Buzzer {
public:
	Buzzer() : updateIntervalMs(10){
		buzzerPin.output(true);
		buzzerPin.set_low();
		currentBeepMsLeft = 0;
		currentState = off;
		for(unsigned int i=0; i< beepsTableSize; i++) {
			beepsQue[i]=0;
		}

	}

	inline void update() volatile {
		if(currentBeepMsLeft > 0) {
			if(currentBeepMsLeft < updateIntervalMs) {
				currentBeepMsLeft = 0;
			} else {
			currentBeepMsLeft -= updateIntervalMs;
			}
			return;
		}

		if(!isQueEmpty())
		{
			if(currentState == on) { //if buzzer just finished playing one beep
				currentBeepMsLeft = beepIntervalMs;
				turnBuzzerOff();
			}
			else {
			currentBeepMsLeft = popFrontBeep();
			turnBuzzerOn();
			}
		} else {
			turnBuzzerOff();
		}

	}

	inline void makeBeep(uint16_t durationMs) volatile {
		unsigned int i=0;
		while(beepsQue[i] != 0 && i < beepsTableSize) {
			i++;
		}
		if(i >= beepsTableSize) {
			return;
		}
		else {
			beepsQue[i] = durationMs;
		}
	}


	const uint16_t updateIntervalMs;
//private:

	inline void turnBuzzerOff() volatile {
		currentState = off;
		buzzerPin.set_low();
	}
	inline void turnBuzzerOn() volatile {
			currentState = on;
			buzzerPin.set_high();
	}
	inline bool isQueEmpty() volatile {
		return (beepsQue[0] == 0);
	}
	inline uint16_t popFrontBeep() volatile {

		if(beepsQue[0] == 0) {
			return 0;
		}

		uint16_t beepDuration = beepsQue[0];
		unsigned int i=0;
		while(beepsQue[i+1] != 0 && i < beepsTableSize) {
			beepsQue[i] = beepsQue[i+1];
			i++;
		}
		if(i < beepsTableSize) {
			beepsQue[i] = 0;
		}
		return beepDuration;
	}

	enum { off, on };

	pin<Port, Pin> buzzerPin;
	uint16_t beepsQue[beepsTableSize];
	uint16_t currentBeepMsLeft;
	uint8_t currentState;
};



#endif /* BUZZER_H_ */
