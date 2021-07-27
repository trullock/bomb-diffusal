#include <Arduino.h>

#define debounceMillis 50
#define BUTTONSTATE_PRESSED 1
#define BUTTONSTATE_RELEASED 2

class Button {

	int pin;
	unsigned long lastMillis = 0;
	bool lastPressed = 0;
	bool pressed = false;

	void updateState()
	{
		bool pressed = digitalRead(this->pin) == HIGH;

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
		}
	}

public:
	Button(int pin, int mode) {
		this->pin = pin;
		
		pinMode(pin, mode);
	}

	bool pressed() {
		this->updateState();
		return pressed;
	}

	bool released() {
		this->updateState();
		return !pressed;
	}
};