#include <slave.h>
#include <serialnumber.h>
#include <Keypad.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

#define STATE_LOCKED 1

class Caeser : public Slave {

	unsigned long nextMillis;
	
	hd44780_I2Cexp lcd;
	Keypad *keypad;

	const uint8_t passwordDifficultyLengths[3] = { 6, 10, 16};
	uint8_t cipher = 0;
	char password[16];
	char plaintext[16];

	char input[16];
	uint8_t inputLength = 0;
	
	void setSerialNumber(byte a, byte b, byte c, byte d, byte e) override
	{
		Slave::setSerialNumber(a, b, c, d, e);

		uint16_t sum = a + b + c + d + e;
		this->cipher = sum % 10;
		
		Serial.print("Cipher: ");
		Serial.println(this->cipher);
	}

	void setDifficulty(byte difficulty) override
	{
		Slave::setDifficulty(difficulty);

		Serial.print("Plaintext: ");

		for(byte i = 0; i < passwordDifficultyLengths[difficulty]; i++)
		{
			this->plaintext[i] = random(0, 10);
			this->password[i] = (this->plaintext[i] + this->cipher) % 10;
			
			Serial.print(serialCharToAscii(this->plaintext[i]));
		}
		Serial.println();
	}

	void arm()
	{
		Slave::arm();
		this->state = STATE_LOCKED;
		this->lcd.backlight();
		this->lcd.blink();
		this->updateDisplay();
	}

	void strike() override
	{
		Slave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;

		this->inputLength = 0;
		this->lcd.clear();
		for(byte i = 0; i < 16; i++)
			this->lcd.write(255);
		this->lcd.setCursor(0, 1);
		for(byte i = 0; i < 16; i++)
			this->lcd.write(255);
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override
	{
		Slave::deactivate();
		this->lcd.clear();
		this->lcd.noBacklight();
	}

	void stopStriking()
	{
		this->state = STATE_LOCKED;
		this->updateDisplay();
	}

	void updateDisplay()
	{
		this->lcd.clear();

		this->lcd.setCursor(0, 0);
		for(byte i = 0; i < passwordDifficultyLengths[difficulty]; i++)
			this->lcd.write(serialCharToAscii(this->password[i]));

		this->lcd.setCursor(0, 1);
		for(byte i = 0; i < this->inputLength; i++)
			this->lcd.write(serialCharToAscii(this->input[i]));
		for(byte i = this->inputLength; i < 16; i++)
			this->lcd.write(' ');
	}

	void handleKeypad()
	{
		char key = this->keypad->getKey();
		if(key)
		{
			if(key == '*')
			{
				// TODO: prevent delete in hard mode?
				
				if(this->inputLength > 0)
				{
					this->inputLength--;
					this->updateDisplay();
				}
				return;
			}

			if(key == '#')
			{
				if(strncmp(this->plaintext, this->input, this->passwordDifficultyLengths[this->difficulty]) == 0)
					this->deactivate();
				else
					this->reportStrike();
				return;
			}

			if(this->inputLength == this->passwordDifficultyLengths[this->difficulty])
				return;

			this->input[this->inputLength] = key - 48;
			this->inputLength++;

			this->updateDisplay();
		}
	}

public:
	Caeser() : 
		Slave(12, 2)
	{
		memset(this->input, 0, 16);
		randomSeed(analogRead(0));

		char keys[4][3] = {
			{'1','2','3'},
			{'4','5','6'},
			{'7','8','9'},
			{'*','0','#'}
		};
		byte rowPins[4] = {3, 8, 7, 5};
		byte colPins[3] = {4, 2, 6};
		this->keypad = new Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

		this->lcd.begin(20, 4);
		this->lcd.clear();
		this->lcd.noBacklight();

		#ifdef DEVMODE
			this->setSerialNumber(0, 0, 0, 0, 1);
			this->setDifficulty(1);
			this->arm();
		#endif
	}

	void loop()
	{
		Slave::loop();

		if(this->state == STATE_LOCKED)
		{
			this->handleKeypad();
		}
		else if(this->state == STATE_STRIKING)
		{
			if(millis() >= nextMillis)
				this->stopStriking();
		}
	}
};