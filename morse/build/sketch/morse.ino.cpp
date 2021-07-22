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

#line 91 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void setup();
#line 117 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void loop();
#line 126 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void handleKnob();
#line 131 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void handleButton();
#line 162 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void updateLed();
#line 218 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void requestEvent();
#line 223 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void arm();
#line 228 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void receiveEvent(int howMany);
#line 91 "d:\\git\\bomb-diffusal\\morse\\morse.ino"
void setup()
{
	// join i2c bus with address #8
	Wire.begin(I2CAddress);
	// enable broadcasts to be received
	TWAR = (I2CAddress << 1) | 1;

  	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);

	Serial.begin(9600);

	Serial.print("I2C Address: ");
	Serial.println(I2CAddress);

	pinMode(LED_Pin, OUTPUT);
	pinMode(Button_Pin, INPUT);

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
		{
			return;
		}

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

void requestEvent() {
	Wire.write(status);
	Wire.write(strikes);
}

void arm()
{
	armed = true;
}

void receiveEvent (int howMany)
{
	
	Serial.print("Received data: ");
	Serial.println(howMany);

	if(howMany == 0)
		return;
	
	byte command = Wire.read();

	Serial.print("Received command: ");
	Serial.println(command);

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
