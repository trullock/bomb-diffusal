#include "AnalogButtons.h"
#include <Arduino.h>

uint8_t AnalogButtons::readBtn()
{
	uint8_t margin = 50;
	int16_t reading = analogRead(A2);
	if(reading < 5 + margin)
		return BTN_LEFT;
	if(reading > 87 - margin && reading < 87 + margin)
		return BTN_UP;
	if(reading > 412 - margin && reading < 412 + margin)
		return BTN_RIGHT;
	if(reading > 236 - margin && reading < 236 + margin)
		return BTN_DOWN;
	if(reading > 637 - margin && reading < 637 + margin)
		return BTN_ENTER;
	return BTN_NONE;
}

AnalogButtons::AnalogButtons(uint8_t pin)
{
	this->pin = pin;
	pinMode(pin, INPUT);
}

uint8_t AnalogButtons::readButton()
{
	// debounce
	if(this->lastBtnMillis + 10 > millis())
		return BTN_NONE;

	uint8_t btn = this->readBtn();
	this->lastBtnMillis = millis();

	if(this->lastBtn == BTN_NONE && this->btnPressed == BTN_NONE && btn == BTN_NONE)
		return BTN_NONE;

	if(btn != this->lastBtn)
	{
		this->buttonReadings = 0;
		this->lastBtn = btn;
		return BTN_NONE;
	}
	
	if(this->buttonReadings < 5)
	{
		this->buttonReadings++;
		return BTN_NONE;
	}

	// we have a new press
	if(btnPressed == BTN_NONE)
	{
		btnPressed = btn;
		return BTN_NONE;
	}
	
	// we have a release
	if(btn == BTN_NONE)
	{
		btn = btnPressed;
		btnPressed = BTN_NONE;
		buttonReadings = 0;
		return btn;
	}
	
	return BTN_NONE;
}