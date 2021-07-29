
#define debounceMillis 50

class Button {

	int pin = 0;
	int mode = 0;
	unsigned long lastMillis = 0;
	bool lastPressed = false;
	bool pressed = false;
	bool read = false;

	void update() {
		bool pressed = digitalRead(this->pin) == this->mode == INPUT ? HIGH : LOW;
		
		if(pressed != lastPressed)
		{
			lastMillis = millis();	
			lastPressed = pressed;
			return;
		}

		if(millis() > (lastMillis + debounceMillis))
		{
			if(pressed == this->pressed)
				return;
			
			this->pressed = pressed;
			this->read = false;
		}
	}

public:
	Button(int pin, int mode)
	{
		pinMode(pin, mode);
		this->pin = pin;
		this->mode = mode;
	}

	bool pressed() {
		this->update();
		return this->pressed;
	}
}