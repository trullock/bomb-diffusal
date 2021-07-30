#include "../lib/slave/slave.h"
#include <Encoder.h>
#include "../lib/button.h"

#define STATE_RECEIVING 1

#define WordCount 5
#define Morse_LED_Pin 7

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

	int correctKnobPosition = 0;

	unsigned long nextMillis = 0;
	bool morseLedOn = false;

	/// Button details
	Button btnEnter;

	//// Knob details
	Encoder knob;
	int knobPosition = 0;
	int frequency = 3550;

	void setDifficulty(int diff)
	{
		Slave::setDifficulty(diff);

		randomSeed(analogRead(0));
		this->currentWordIndex = random(0, WordCount);
		this->currentWord = words[difficulty][this->currentWordIndex];
		Serial.print("Current word: ");
		Serial.println(this->currentWord);

		this->correctKnobPosition = difficulty * (this->currentWordIndex + 1) * 3;
	}

	void arm()
	{
		Slave::arm();

		this->state = STATE_RECEIVING;
	}

	void strike() override {
		Slave::strike();
	}

	void explode() override
	{
		Slave::explode();
	}

	void deactivate() override {
		Slave::deactivate();
	}

public:
	Morse() : Slave(6), btnEnter(5, INPUT), knob(2, 3)
	{
		pinMode(Morse_LED_Pin, OUTPUT);
	}

	void loop()
	{
		if(this->state == STATE_RECEIVING)
		{
			this->handleButton();
			this->handleKnob();

			this->updateLed();
			this->updateDisplay();
		}
	}

	void handleKnob()
	{
		int position = knob.read();
		if (position != knobPosition)
			knobPosition = position;
	}

	void handleButton()
	{
		if(!this->btnEnter.pressed())
			return;
		
		if (knobPosition == correctKnobPosition)
		{
			this->deactivate();
			return;
		}

		this->reportStrike();
	}

	void updateDisplay()
	{
		int frequency = 3000 + (knobPosition * 50);
		// TODO: implement
		//required range is 15*3
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
};