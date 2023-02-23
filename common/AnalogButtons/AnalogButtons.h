#ifndef ANALOGBUTTONS_H_
#define ANALOGBUTTONS_H_

#include <Arduino.h>

#define BTN_NONE 0
#define BTN_LEFT 1
#define BTN_UP 2
#define BTN_RIGHT 3
#define BTN_DOWN 4
#define BTN_ENTER 5

class AnalogButtons
{
	uint8_t lastBtn = BTN_NONE;
	uint8_t btnPressed = BTN_NONE;
	unsigned long lastBtnMillis = 0;
	uint8_t buttonReadings = 0;
	uint8_t pin;

	uint8_t readBtn();

public:

	AnalogButtons(uint8_t pin);
	uint8_t readButton();
};

#endif