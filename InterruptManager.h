/*
 * InterruptManager.h
 *
 *  Created on: 7 lis 2014
 *      Author: Lukasz
 */

#ifndef INTERRUPTMANAGER_H_
#define INTERRUPTMANAGER_H_

#include "PomodoroTimer.h"
#include "LedBar.h"
#include "uart.h"

//extern volatile LedBar ledBar;
extern volatile PomodoroTimer pomodoroTimer;

class InterruptManager {
public:
	InterruptManager() {
		ledBarCounter = pomodoroTimerCounter = buttonCounter = signalLedsCounter = buzzerCounter =0;
	}

	inline void update() volatile {
		if(ledBarCounter == pomodoroTimer.ledBar.updateIntervalMs * 8U) {
			resetCounter(ledBarCounter);
			pomodoroTimer.ledBar.update();
		}
		if(pomodoroTimerCounter == pomodoroTimer.updateIntervalMs * 8U) {
			resetCounter(pomodoroTimerCounter);
			pomodoroTimer.update();
		}
		if(buttonCounter == pomodoroTimer.button1.updateIntervalMs * 8U) {
			resetCounter(buttonCounter);
			pomodoroTimer.button1.update();
		}
		if(signalLedsCounter == pomodoroTimer.breakLed.updateIntervalMs * 8U) {
			resetCounter(signalLedsCounter);
			pomodoroTimer.breakLed.update();
			pomodoroTimer.workLed.update();
		}
		if(buzzerCounter == pomodoroTimer.buzzer.updateIntervalMs * 8U) {
			resetCounter(buzzerCounter);
			pomodoroTimer.buzzer.update();
		}
		pomodoroTimer.ledBar.ledsController.update();
		incrementCounters();
	}
private:
	inline void incrementCounters() volatile{
		ledBarCounter++;
		pomodoroTimerCounter++;
		buttonCounter++;
		signalLedsCounter++;
		buzzerCounter++;
	}

	inline void resetCounter(volatile uint16_t &counter) volatile {
		counter = 0;
	}

	uint16_t ledBarCounter;
	uint16_t pomodoroTimerCounter;
	uint16_t buttonCounter;
	uint16_t signalLedsCounter;
	uint16_t buzzerCounter;

};



#endif /* INTERRUPTMANAGER_H_ */
