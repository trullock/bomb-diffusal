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

#define WordCount 2
const String words[WordCount] = {
	"alpha",
	"bravo"
};

String currentWord;
char currentChar;
int currentCharIndex = -1;
String currentMorseChar;
int currentMorseIndex = -1;

bool ledOn = false;
unsigned long nextMillis = 0;

#define LED_Pin 7

unsigned long lastButtonMillis = 0;
bool lastButtonPressed = false;
bool buttonPressed = false;
#define debounceMillis 50
#define Button_Pin 5

bool armed = false;
byte difficulty = 1;

#define COMMAND_ARM 1
#define COMMAND_DIFFICULTY 2

#define DIFFICULTY_LOW 1
#define DIFFICULTY_MED 2
#define DIFFICULTY_HIGH 3

void setup()
{
	pinMode(LED_Pin, OUTPUT);
	pinMode(Button_Pin, INPUT);

	Wire.begin(I2CAddress);
	// enable broadcasts to be received
	TWAR = (I2CAddress << 1) | 1;

  	Wire.onRequest(reportStatus);
	Wire.onReceive(receiveCommand);

	Serial.begin(9600);

	Serial.print("I2C Address: ");
	Serial.println(I2CAddress);

	randomSeed(analogRead(0));
	int wordIndex = random(0, WordCount);
	currentWord = words[wordIndex];
	Serial.print("Current word: ");
	Serial.println(currentWord);
}

void loop()
{
	if(!armed)
		return;

	updateLed();
	handleButton();
}

void handleKnob() {
	int position = knob.read();
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
	nextMillis = millis() + (blipIsDot ? dotLength : dashLength);

	ledOn = true;
	digitalWrite(LED_Pin, HIGH);
}

void reportStatus() {
	Wire.write(status);
	Wire.write(strikes);
}

void arm()
{
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