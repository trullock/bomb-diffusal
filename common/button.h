#include <Arduino.h>

#ifndef Button_h
#define Button_h

class Button {

	int pin;
	int mode;
	unsigned long lastMillis = 0;
	bool lastPressed = 0;
	bool isPressed = false;
	bool read = true;

	void updateState()
	{
		bool pressed = digitalRead(this->pin) == (mode == INPUT ? HIGH : LOW);

		if (pressed != this->lastPressed)
		{
			this->lastMillis = millis();
			this->lastPressed = pressed;
			return;
		}

		if (millis() > (this->lastMillis + 50))
		{
			if (pressed == this->isPressed)
				return;

			this->isPressed = pressed;
			this->read = false;
		}
	}

public:
	Button(int pin, int mode) {
		this->pin = pin;
		this->mode = mode;
		
		pinMode(pin, mode);
	}

	bool pressed() {
		this->updateState();

		if(this->read)
			return false;

		this->read = true;
		return isPressed;
	}

	bool released() {
		this->updateState();
		
		if(this->read)
			return false;

		this->read = true;
		return !isPressed;
	}
};

#endif