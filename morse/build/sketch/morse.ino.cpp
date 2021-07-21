#line 1 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
#include <Wire.h>
#include <Encoder.h>
#include <Arduino.h>

byte strikes = 0;
byte status = 0;

#define I2CAddress 8

Encoder knob(2, 3);
int knobPosition;

const String morseAlphabet[] = {
	".-",	// a
	"-...",	// b
	"-.-.",	// c
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
	"-----",	// 0
	".----",	// 1
	"..---",	// 2
	"...--",
	"....-",
	".....",
	"-....",
	"--...",
	"---..",
	"----."
};

int dotLength = 400;
int dashLength = 800;
int blipDelay = 200;
int characterDelay = 1000;
int loopDelay = 3000;

#define WordCount 1
const String words[WordCount] = {
	"sos"
};

String currentWord;
char currentChar;
int currentCharIndex = -1;
String currentMorseChar;
int currentMorseIndex = -1;

bool ledOn = false;
unsigned long nextMillis = 0;

#define LED_Pin LED_BUILTIN

#line 74 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void setup();
#line 95 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void loop();
#line 100 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void handleKnob();
#line 104 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void updateLed();
#line 161 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void requestEvent();
#line 74 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void setup()
{
	// join i2c bus with address #8
	//Wire.begin(I2CAddress);
  	//Wire.onRequest(requestEvent);

	Serial.begin(9600);

	Serial.print("I2C Address: ");
	Serial.println(I2CAddress);

	pinMode(LED_Pin, OUTPUT);

	randomSeed(analogRead(0));
	int wordIndex = random(0, WordCount);
	currentWord = words[wordIndex];

	Serial.print("Current word: ");
	Serial.println(currentWord);
}

void loop()
{
	updateLed();
}

void handleKnob() {
	int position = knob.read();
}

void updateLed()
{
	if(millis() < nextMillis)
		return;

	// If the LED is currently ON
	if(ledOn) {
		// turn LED off
		digitalWrite(LED_Pin, LOW);
		ledOn = false;

		// if we're at the end of a character
		if(currentMorseIndex == currentMorseChar.length() - 1)
		{
			// if it was the last char in the word
			if(currentCharIndex == currentWord.length() - 1)
				nextMillis = millis() + loopDelay;
			else
				nextMillis = millis() + characterDelay;
		}
		else
		{
			nextMillis = millis() + blipDelay;
		}

		currentMorseIndex++;

		return;
	}

	// else LED if currently OFF

	// if we're just starting or we've finished this character
	if(currentMorseChar == "" || currentMorseIndex == currentMorseChar.length())
	{
		// if it was the last char of the word
		if(currentCharIndex == currentWord.length() - 1)
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
	Serial.println(blipIsDot ? "Dot" : "Dash");
	nextMillis = millis() + (blipIsDot ? dotLength : dashLength);

	ledOn = true;
	digitalWrite(LED_Pin, HIGH);
}

void requestEvent() {
	Wire.write(status);
	Wire.write(strikes);
}
