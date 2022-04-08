#include <Arduino.h>
#include "button.h"

#ifndef Joystick_h
#define Joystick_h

#define JOYSTICK_NONE 0
#define JOYSTICK_NORTH 1
#define JOYSTICK_EAST 2
#define JOYSTICK_SOUTH 4
#define JOYSTICK_WEST 8

/**
 * Handles an x-y joystick with switch
 */
class Joystick {

	uint8_t xPin;
	uint8_t yPin;
	uint8_t xCenter;
	uint8_t yCenter;

	bool read;

	Button btn;
	
	int x;
	int y;


	void calibrate()
	{
		this->updateState();
		this->xCenter = this->x;
		this->yCenter = this->y;
	}

public:

	/**
	 * Creates a new Joystick for the given pins
	 */
	Joystick(uint8_t xPin, uint8_t yPin, uint8_t buttonPin) : btn(buttonPin, INPUT_PULLUP) {
		this->xPin = xPin;
		this->yPin = yPin;

		pinMode(xPin, INPUT);
		pinMode(yPin, INPUT);

		this->calibrate();
	}

	/**
	 * Updates the state of the button.
	 * If the state changes, this is recorded and `read = false`
	 */
	void updateState()
	{
		uint16_t xTotal = 0;
		uint16_t yTotal = 0;

		for(uint8_t i = 0; i < 10; i ++)
		{
			xTotal += analogRead(this->xPin);
			yTotal += analogRead(this->yPin);
			delay(10);
		}

		this->x = xTotal / 10;
		this->y = yTotal / 10;

		this->read = false;
	}

	/**
	 * Determines if the button has been pressed.
	 * Calling this method will only return `true` once for each press.
	 * This will return true if the button is pressed when this class was instantiated
	 */
	bool pressed() {
		return this->btn.pressed();
	}

	/**
	 * Determines if the button has been released after being pressed.
	 * Calling this method will only return `true` once for each release.
	 * This will not return true if the button is released when this class was instantiated
	 */
	bool released() {
		return this->btn.released();
	}

	int direction() {
		Serial.println(this->y);
		uint8_t value = JOYSTICK_NONE;

		if(this->y > 900)
			value |= JOYSTICK_NORTH;
		if(this->x > 900)
			value |= JOYSTICK_EAST;
		if(this->y < 100)
			value |= JOYSTICK_SOUTH;
		if(this->y < 100)
			value |= JOYSTICK_WEST;

		return value;
	}
};

#endif