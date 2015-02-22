/*
 * LedBar.h
 *
 *  Created on: 7 lis 2014
 *      Author: Lukasz
 */

#ifndef LEDBAR_H_
#define LEDBAR_H_
#include "uart.h"



const int ledBarNumberOfLeds = 5;
const int MAX_DUTY = 99;
class PWMController {
public:
	PWMController(uint8_t *dutyTable, uint8_t *portsTable, uint8_t *pinsTable);
	inline void update() volatile{
		for(uint8_t i = 0; i < ledBarNumberOfLeds; i++) {
			if(pinsDutyCycle[i] >= counter) {
				PORTD |= (1<<pins[i]);
			}
			else {
				PORTD &=~(1<<pins[i]);
			}
		}
		if(counter == 99) {
			counter = 0;
		}

		counter++;
	}
private:

	uint8_t *pinsDutyCycle;
	uint8_t *ports;
	uint8_t *pins;
	uint8_t counter;
};

PWMController::PWMController(uint8_t *dutyTable, uint8_t *portsTable, uint8_t *pinsTable) {
	pinsDutyCycle = dutyTable;
	ports = portsTable;
	pins = pinsTable;
	counter = 0;
}

class LedBar {
public:
	LedBar();
	void setTimeLeft(uint16_t secondsLeft, uint16_t maxSeconds) volatile;
	void update() volatile;
	const uint8_t updateIntervalMs;
	PWMController ledsController;
//private:
	void setOutputs();

	const uint8_t maxDutyCycle;
	uint8_t ledDutyCycle[ledBarNumberOfLeds];
	uint8_t ledPins[ledBarNumberOfLeds];
	uint8_t ledPorts[ledBarNumberOfLeds];

};

LedBar::LedBar() : updateIntervalMs(10), ledsController(ledDutyCycle, ledPorts, ledPins), maxDutyCycle(MAX_DUTY)  {
	setOutputs();
}

void LedBar::setTimeLeft(uint16_t secondsLeft, uint16_t maxSeconds) volatile {
	int timePerOneLed = maxSeconds/ledBarNumberOfLeds;
	uint8_t numberOfLedsFullyOn = secondsLeft / timePerOneLed;
	for(int i = 0; i < numberOfLedsFullyOn ; i++) {
		ledDutyCycle[i] = maxDutyCycle;
	}
	if(numberOfLedsFullyOn == ledBarNumberOfLeds) {
		return;
	}
	uint16_t secondsCountForFadingLed = secondsLeft - numberOfLedsFullyOn*timePerOneLed;
	uint8_t fadingLedDutyCycle = (secondsCountForFadingLed * maxDutyCycle)/timePerOneLed;
	ledDutyCycle[numberOfLedsFullyOn] = fadingLedDutyCycle;

	for(int i = numberOfLedsFullyOn + 1; i < ledBarNumberOfLeds; i++) {
		ledDutyCycle[i] = 0;
	}
}

void LedBar::update() volatile{

}

void LedBar::setOutputs() {
	for(int i=0; i< ledBarNumberOfLeds; i++) {
		ledPorts[i] = PORTD;
		ledPins[i] = i+3;
		ledDutyCycle[i] = 255;

		DDRD |= (1<<ledPins[i]); //setting all pins as outputs
	}
}




#endif /* LEDBAR_H_ */
