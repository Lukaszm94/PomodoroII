/*
 * PomodoroTimer.h
 *
 *  Created on: 7 lis 2014
 *      Author: Lukasz
 */

#ifndef POMODOROTIMER_H_
#define POMODOROTIMER_H_
#include "LedBar.h"
#include "Button.h"
#include "Led.h"
#include "Buzzer.h"
#include "uart.h"


const uint16_t workModeMaxSeconds = 1500;
const uint16_t breakModeMaxSeconds = 300;
const uint16_t shortPressMaxTime = 1000; //ms
const uint16_t pausedSignalLedBlinkPeriodMs = 1000;
const uint16_t shortBeep = 50;
const uint16_t mediumBeep = 500;
const uint16_t longBeep = 700;

#define testLed (1<<PB0)
class PomodoroTimer {
public:
	PomodoroTimer();
	void update() volatile; //called every 100ms
	void shortButtonPress();
	void longButtonPress();
	bool secondElapsed;
	const uint8_t updateIntervalMs;
	LedBar ledBar;
	Button button1;
	Led<portc, 4> workLed;
	Led<portc, 5> breakLed;
	Buzzer<portc, 3> buzzer;
//private:
	void onSecondElapsed() volatile;
	uint16_t getCurrentModeMaxTime() volatile;
	void toggleMode() volatile;
	void toggleModeLeds(uint8_t) volatile;
	void resetCounters() volatile;
	void buzzerAction() volatile; //this function is called at THE END of certain mode
	void pause() volatile;
	void resume() volatile;
	void restart() volatile;
	enum { Work, Break, Running, Paused };
	uint16_t counter100ms;
	uint16_t counterSeconds;
	uint8_t currentMode;
	uint8_t currentState;


};

PomodoroTimer::PomodoroTimer() : updateIntervalMs(100), button1(shortPressMaxTime) {
	counter100ms = counterSeconds = 0;
	currentMode = Work;
	workLed.turnOn();
	breakLed.turnOff();
	secondElapsed = false;
	pause();
}

void PomodoroTimer::update() volatile {
	uint8_t lastButtonAction = button1.getLastAction();
	if(lastButtonAction != button1.noAction) {
		uart_putc('b');
		if(lastButtonAction == Button::shortPress) {
			if(currentState == Running) {
				pause();
			}
			else if(currentState == Paused) {
				resume();
			}
		}
		else if(lastButtonAction == Button::longPress) {
			restart();
		}
		button1.clearAction();
	}

	if(currentState == Paused)
		return;

	if(counter100ms == 10) {
		counter100ms = 0;
		counterSeconds++;
		onSecondElapsed();
	}



	counter100ms++;
}

void PomodoroTimer::onSecondElapsed() volatile {
	if(counterSeconds == getCurrentModeMaxTime()) {
		buzzerAction();
		toggleMode();
		toggleModeLeds(currentMode); //FIXME
		resetCounters();
	}
	ledBar.setTimeLeft(getCurrentModeMaxTime() - counterSeconds, getCurrentModeMaxTime());
	if(currentMode == Work && (counterSeconds%(workModeMaxSeconds/ledBarNumberOfLeds) == 0) && counterSeconds!=0) {
		buzzer.makeBeep(shortBeep);
	}
	if(currentMode == Break && (counterSeconds%(breakModeMaxSeconds/ledBarNumberOfLeds) == 0) && counterSeconds!=0) {
		buzzer.makeBeep(shortBeep);
	}
	secondElapsed = true;

}

uint16_t PomodoroTimer::getCurrentModeMaxTime() volatile{
	if(currentMode == Work) {
		return workModeMaxSeconds;
	}
	else if(currentMode == Break) {
		return breakModeMaxSeconds;
	}
	return 1; //just to kill the warning
}

void PomodoroTimer::toggleMode() volatile {
	if(currentMode == Work) {
		currentMode = Break;
	}
	else if(currentMode == Break) {
		currentMode = Work;
	}
}

void PomodoroTimer::toggleModeLeds(uint8_t mode) volatile {
	if(mode == Work) {
		workLed.turnOn();
		breakLed.turnOff();
	}
	else if(mode == Break) {
		workLed.turnOff();
		breakLed.turnOn();
	}
}

void PomodoroTimer::resetCounters() volatile {
	counter100ms = 0;
	counterSeconds = 0;
}

void PomodoroTimer::buzzerAction() volatile {
	if(currentMode == Work) {
		buzzer.makeBeep(mediumBeep);
		buzzer.makeBeep(mediumBeep);
		buzzer.makeBeep(longBeep);
	}
	else if(currentMode == Break) {
		buzzer.makeBeep(longBeep);
	}
}

void PomodoroTimer::pause() volatile {
	currentState = Paused;
	buzzer.makeBeep(shortBeep);
	if(currentMode == Work) {
		workLed.setMode(workLed.blinking);
		workLed.setBlinkingRate(pausedSignalLedBlinkPeriodMs);
	}
	else if(currentMode == Break) {
		breakLed.setMode(breakLed.blinking);
		breakLed.setBlinkingRate(pausedSignalLedBlinkPeriodMs);
	}
}

void PomodoroTimer::resume() volatile {
	currentState = Running;
	buzzer.makeBeep(shortBeep);
	if(currentMode == Work) {
		workLed.setMode(workLed.normal);
		workLed.turnOn();
	}
	else if(currentMode == Break) {
		breakLed.setMode(breakLed.normal);
		breakLed.turnOn();
	}
}

void PomodoroTimer::restart() volatile {
	resetCounters();
	breakLed.setMode(breakLed.normal);
	breakLed.turnOff();
	currentMode = Work;
	workLed.turnOn();
	ledBar.setTimeLeft(getCurrentModeMaxTime(), getCurrentModeMaxTime());
	pause();
}

#endif /* POMODOROTIMER_H_ */
