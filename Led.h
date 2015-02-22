/*
 * Led.h
 *
 *  Created on: 9 lis 2014
 *      Author: Lukasz
 */

#ifndef LED_H_
#define LED_H_
#include "pin.hpp"
#include "portc.hpp"
using namespace avrlib;



template <typename Port, int Pin>
class Led {
public:
	Led() : updateIntervalMs(100), blinkingPeriodMs(1000) {
		ledPin.output(true);
		currentMode = normal;
		counter = 0;
	}

	enum { normal, blinking	};
	const uint16_t updateIntervalMs;
	inline void update() volatile {
		if(currentMode != blinking) {
			return;
		}

		if(counter == blinkingPeriodMs/2) {
			ledPin.toggle();
			counter = 0;
		}
		counter += updateIntervalMs;
	}
	inline void turnOn() volatile {
		ledPin.set_high();
	}
	inline void turnOff() volatile {
		ledPin.set_low();
	}

	inline void setMode(uint8_t newMode) volatile {
		currentMode = newMode;
		counter = 0;
	}
	inline void setBlinkingRate(uint16_t periodMs) volatile {
		blinkingPeriodMs = periodMs;
	}


//private:
	inv_pin<Port, Pin> ledPin;
	uint8_t currentMode;
	uint16_t blinkingPeriodMs;
	uint16_t counter;
};



#endif /* LED_H_ */
