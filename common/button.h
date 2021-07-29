#include <Arduino.h>

#define debounceMillis 50
#define BUTTONSTATE_PRESSED 1
#define BUTTONSTATE_RELEASED 2

class Button {

	int pin;
	int mode;
	unsigned long lastMillis = 0;
	bool lastPressed = 0;
	bool pressed = false;
	bool read = false;

	void updateState()
	{
		bool pressed = digitalRead(this->pin) == mode == INPUT ? HIGH : LOW;

		if (pressed != this->lastPressed)
		{
			this->lastMillis = millis();
			this->lastPressed = pressed;
			return;
		}

		if (millis() > (this->lastMillis + debounceMillis))
		{
			if (pressed == this->pressed)
				return;

			this->pressed = pressed;
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
		return pressed;
	}

	bool released() {
		this->updateState();
		if(this->read)
			return false;
		this->read = true;
		return !pressed;
	}
};