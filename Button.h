/*
 * Button.h
 *
 *  Created on: 9 lis 2014
 *      Author: Lukasz
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "portb.hpp"
#include "pin.hpp"
#include "uart.h"

using namespace avrlib;
typedef pin<portb, 0> buttonPin;

class Button {

public:
	Button(const uint16_t _shortPressMaxTime);
	inline void update() volatile { //called every 5ms
		if(counterState == debouncing) {
			if(pressLengthMs >= debounceDelay) {
				counterState = counting;
				pressLengthMs = 0;
				return;
			}
			pressLengthMs += updateIntervalMs;
			return;
		}
		if(isButtonPressed()) {
			if(counterState == notCounting) {
				counterState = debouncing;
				return;
			}
			if(counterState == counting) {
				pressLengthMs += updateIntervalMs;
				return;
			}
		}
		else { //when button is released
			if(counterState == counting) {
				if(pressLengthMs <= shortPressMaxTime) {
					lastAction = shortPress;
					counterState = notCounting;
					pressLengthMs = 0;
				}
				else if(pressLengthMs > shortPressMaxTime) {
					lastAction = longPress;
					counterState = notCounting;
					pressLengthMs = 0;
				}
			}
			else {
				pressLengthMs = 0;
				counterState = notCounting;
			}
		}
	}

	inline uint8_t getLastAction() volatile {
		return lastAction;
	}

	inline void clearAction() volatile {
		lastAction = noAction;
	}

	inline bool isButtonPressed() volatile {
		return !buttonPin::value();
	}
	enum {noAction = 0, shortPress, longPress, notCounting, debouncing, counting};
	const uint8_t updateIntervalMs;
private:
	const uint16_t debounceDelay;
	const uint16_t shortPressMaxTime;
	uint8_t lastAction;
	uint16_t pressLengthMs;
	uint8_t counterState;

};

Button::Button(const uint16_t _shortPressMaxTime) : updateIntervalMs(5), debounceDelay(30), shortPressMaxTime(_shortPressMaxTime) {
	buttonPin::make_input();
	buttonPin::set_high();
	lastAction = noAction;
	pressLengthMs = 0;
	counterState = notCounting;
}

#endif /* BUTTON_H_ */
