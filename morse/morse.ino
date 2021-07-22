#include <Wire.h>
#include <Encoder.h>
#include <Arduino.h>

#define I2CAddress 8

/// General Module details

byte strikes = 0;
byte status = 0;

bool armed = false;
byte difficulty = 1;

bool disarmed = false;
#define Disarmed_LED_PIN 8

byte pendingNotification = 0;

#define COMMAND_ARM 1
#define COMMAND_DIFFICULTY 2

#define DIFFICULTY_LOW 1
#define DIFFICULTY_MED 2
#define DIFFICULTY_HIGH 3

/// Specific Module details

//// Morse details

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

// morse settings for Easy, Med, Hard
const int dotLength[] = { 500, 400, 300 };
const int dashLength[] = { 900, 800, 700 };
const int blipDelay[] = { 300, 200, 100 };
const int characterDelay[] = { 1200, 1000, 800 };
const int loopDelay[]  = { 3000, 3000, 2500 };

#define WordCount 5
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

String currentWord;
char currentChar;
int currentCharIndex = -1;
String currentMorseChar;
int currentMorseIndex = -1;

#define Morse_LED_Pin 7
bool morseLedOn = false;

unsigned long nextMillis = 0;

//// Button details
unsigned long lastButtonMillis = 0;
bool lastButtonPressed = false;
bool buttonPressed = false;
#define debounceMillis 50
#define Button_Pin 5

//// Knob details
Encoder knob(2, 3);
int knobPosition = 0;
int frequency = 3550;



void setup()
{
	pinMode(Morse_LED_Pin, OUTPUT);
	pinMode(Button_Pin, INPUT);

	Wire.begin(I2CAddress);
	// enable broadcasts to be received
	TWAR = (I2CAddress << 1) | 1;

  	Wire.onRequest(reportStatus);
	Wire.onReceive(receiveCommand);

	Serial.begin(9600);

	Serial.print("Morse module joining I2C bus with address: ");
	Serial.println(I2CAddress);
}

void loop()
{
	renderFreq();

	if(!armed)
		return;

	updateLed();
	handleButton();
	handleKnob();
}

void handleKnob() {
	int position = knob.read();
	if(position != knobPosition)
	{
		knobPosition = position;
		frequency = max(3500, min(3600, (position + 10) * 5));
	}
}

void renderFreq()
{
	// TODO: implement
}

void handleButton() {
	bool pressed = digitalRead(Button_Pin) == HIGH;
	
	if(pressed != lastButtonPressed)
	{
		lastButtonMillis = millis();	
		lastButtonPressed = pressed;
		return;
	}

	if(millis() > (lastButtonMillis + debounceMillis))
	{
		if(pressed == buttonPressed)
			return;
		
		buttonPressed = pressed;

		if(!pressed)
			return;
		
		// TODO: implement properly
		
		strikes++;
		Serial.println("Striking");
	}
}

void updateLed()
{
	if(millis() < nextMillis)
		return;

	// If the LED is currently ON
	if(morseLedOn) {
		// turn LED off
		digitalWrite(Morse_LED_Pin, LOW);
		morseLedOn = false;

		// if we're at the end of a character
		if(currentMorseIndex == currentMorseChar.length() - 1)
		{
			// if it was the last char in the word
			if(currentCharIndex == currentWord.length() - 1)
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
	nextMillis = millis() + (blipIsDot ? dotLength[difficulty] : dashLength[difficulty]);

	morseLedOn = true;
	digitalWrite(Morse_LED_Pin, HIGH);
}

void reportStatus() {
	Wire.write(status);
	Wire.write(strikes);
	Wire.write(pendingNotification);
	pendingNotification = 0;
}

void arm()
{
	randomSeed(analogRead(0));
	int wordIndex = random(0, WordCount);
	currentWord = words[difficulty][wordIndex];
	Serial.print("Current word: ");
	Serial.println(currentWord);

	armed = true;
}

void receiveCommand (int howMany)
{
	// Master scanning for modules will cause this
	if(howMany == 0)
		return;
	
	byte command = Wire.read();

	if(command == COMMAND_DIFFICULTY)
	{
		difficulty = Wire.read();
		
		Serial.print("Setting difficulty to: ");
		Serial.println(difficulty);

		if(armed)
			arm();
	}
	else if(command == COMMAND_ARM)
	{
		Serial.println("Arming");
		arm();
	}
}