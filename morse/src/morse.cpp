#include "../lib/slave/slave.h"
#include <Encoder.h>
#include "../lib/button.h"
#include "../lib/LedControl/LedControl.h"

#define STATE_MORSING 1

#define WordCount 5
#define Morse_LED_Pin 5

class Morse : public Slave {

	const String morseAlphabet[36] = {
		".-",	// a
		"-...", // b
		"-.-.", // c
		"-..",	// d
		".",
		"..-.",
		"--.",
		"....",
		"..",
		".---",
		"-.-",
		".-..",
		"--",
		"-.",
		"---",
		".--.",
		"--.-",
		".-.",
		"...",
		"-",
		"..-",
		"...-",
		".--",
		"-..-",
		"-.--",
		"--..",
		"-----", // 0
		".----", // 1
		"..---", // 2
		"...--",
		"....-",
		".....",
		"-....",
		"--...",
		"---..",
		"----."
	};

	// morse settings for Easy, Med, Hard
	const int dotLength[3] = {500, 400, 300};
	const int dashLength[3] = {900, 800, 700};
	const int blipDelay[3] = {300, 200, 100};
	const int characterDelay[3] = {1200, 1000, 800};
	const int loopDelay[3] = {3000, 3000, 2500};
	const int freqMultiplier[3] { 100, 200, 300};
	const int startFreq[3] { 2250, 2250, 2250 };

	const String words[3][WordCount] = {
		{
			"echo",
			"golf",
			"kilo",
			"lima",
			"mike"
		},
		{
			"alpha",
			"bravo",
			"delta",
			"tango",
			"oscar",
		},
		{
			"charlie",
			"foxtrot",
			"november",
			"quebec",
			"whiskey"
		 }
	};

	int currentWordIndex = 0;
	String currentWord;
	char currentChar;
	int currentCharIndex = -1;
	String currentMorseChar;
	int currentMorseIndex = -1;


	unsigned long nextMillis = 0;
	bool morseLedOn = false;

	/// Button details
	Button btnEnter;

	//// Knob details
	Encoder knob;
	int knobPosition = 0;
	int correctFrequency = 0;
	int currentFrequency = 0;
	const int knobStep[3] = {10, 5, 1};

	// Display
	LedControl display;

	void setDifficulty(byte diff) override
	{
		Slave::setDifficulty(diff);

		randomSeed(analogRead(0));
		this->currentWordIndex = random(0, WordCount);
		this->currentWord = words[difficulty][this->currentWordIndex];
		Serial.print("Current word: ");
		Serial.println(this->currentWord);

		this->currentFrequency = startFreq[difficulty];
		this->correctFrequency = startFreq[difficulty] + (this->currentWordIndex + 1) * freqMultiplier[difficulty];

		Serial.print("Correct frequency: ");
		Serial.println(this->correctFrequency);
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_MORSING;
	}

	void strike() override {
		Slave::strike();

		this->nextMillis = millis() + STRIKE_DURATION_MS;
		this->setLed(true);

		display.setDigit(0, 0, 8, true);
		display.setDigit(0, 1, 8, true);
		display.setDigit(0, 2, 8, true);
		display.setDigit(0, 3, 8, true);
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override
	{
		Slave::deactivate();

		setLed(false);
		display.setIntensity(0, 1);
		display.setChar(0, 0, '-', false);
		display.setChar(0, 1, '-', false);
		display.setChar(0, 2, '-', false);
		display.setChar(0, 3, '-', false);
	}

	void handleKnob()
	{
		int position = knob.read();
		if (position != knobPosition)
		{
			knobPosition = position;

			currentFrequency = startFreq[difficulty] + position * knobStep[difficulty];
		}
	}

	void handleButton()
	{
		if (!this->btnEnter.pressed())
			return;

		if (currentFrequency == correctFrequency)
		{
			this->deactivate();
			return;
		}

		this->reportStrike();
	}

	void updateDisplay()
	{
		int thousands = currentFrequency / 1000;
		int hundreds = (currentFrequency - thousands * 1000) / 100;
		int tens = (currentFrequency - thousands * 1000 - hundreds * 100) / 10;
		int ones = currentFrequency % 10;

		display.setDigit(0, 0, ones, false);
		display.setDigit(0, 1, tens, false);
		display.setDigit(0, 2, hundreds, false);
		display.setDigit(0, 3, thousands, true);
	}

	void setLed(bool on)
	{
		morseLedOn = on;
		digitalWrite(Morse_LED_Pin, on ? HIGH : LOW);
	}

	void updateLed()
	{
		if (millis() < nextMillis)
			return;

		// If the LED is currently ON
		if (morseLedOn)
		{
			this->setLed(false);

			// if we're at the end of a character
			if (currentMorseIndex == currentMorseChar.length() - 1)
			{
				// if it was the last char in the word
				if (currentCharIndex == currentWord.length() - 1)
					nextMillis = millis() + loopDelay[difficulty];
				else
					nextMillis = millis() + characterDelay[difficulty];
			}
			else
			{
				nextMillis = millis() + blipDelay[difficulty];
			}

			currentMorseIndex++;

			return;
		}

		// else LED if currently OFF

		// if we're just starting or we've finished this character
		if (currentMorseChar == "" || currentMorseIndex == currentMorseChar.length())
		{
			// if it was the last char of the word
			if (currentCharIndex == currentWord.length() - 1)
				currentCharIndex = 0;
			else
				currentCharIndex++;

			currentMorseIndex = 0;

			// get the next char in alpha and morse
			currentChar = currentWord[currentCharIndex];
			currentMorseChar = morseAlphabet[currentChar - 97];
		}

		// wait for dot/dash length of time
		bool blipIsDot = currentMorseChar[currentMorseIndex] == '.';
		nextMillis = millis() + (blipIsDot ? dotLength[difficulty] : dashLength[difficulty]);

		this->setLed(true);
	}

	void stopStriking()
	{
		this->state = STATE_MORSING;
		this->setLed(false);
		this->currentMorseChar = "";
		this->currentCharIndex = -1;
		this->nextMillis = millis() + this->loopDelay[this->difficulty];
	}

public:
	Morse() : 
		Slave(5), 
		btnEnter(8, INPUT_PULLUP), 
		knob(6, 7),
		display(12, 13, 10, 1)
	{
		pinMode(Morse_LED_Pin, OUTPUT);

		this->display.shutdown(0, false);
		this->display.setIntensity(0, 8);
		this->display.clearDisplay(0);
	}

	void loop()
	{
		Slave::loop();

		if(this->state == STATE_MORSING)
		{
			this->updateLed();
			this->updateDisplay();

			this->handleButton();
			this->handleKnob();
		}
		else if(this->state == STATE_STRIKING)
		{
			if(millis() >= nextMillis)
				this->stopStriking();
		}
	}
};