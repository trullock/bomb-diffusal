#include <Wire.h>
#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define I2CAddress 7

/// General Module details

byte strikes = 0;
byte status = 0;

bool armed = false;
byte difficulty = 1;

bool deactivated = false;
#define Disarmed_LED_PIN 8

byte pendingNotification = 0;

#define COMMAND_ARM 1
#define COMMAND_DIFFICULTY 2
#define COMMAND_EXPLODE 3

#define DIFFICULTY_LOW 1
#define DIFFICULTY_MED 2
#define DIFFICULTY_HIGH 3

#define NOTIFICATION_DISARMED 1

/// Specific Module details

//// Maze details

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 3

// https://www.makerguides.com/max7219-led-dot-matrix-display-arduino-tutorial/
// https://majicdesigns.github.io/MD_MAX72XX/class_m_d___m_a_x72_x_x.html
MD_MAX72XX display(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

int posX = 0,
	 posY = 0;

int clue1X = 1, clue1Y = 4,
	clue2X = 5, clue2Y = 7;
int startX = 4, startY = 3;
bool maze[8+7][8+7] = {
  //     |     |     |     |     |     |     |
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // -
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

#define STATE_CLUE 1
#define STATE_NAVIGATING 2
int state = 0;

unsigned long nextMillis = 0;

#define Button_Pin 1
#define debounceMillis 50
unsigned long lastButtonMillis = 0;
bool lastButtonPressed = false;
bool buttonPressed = false;

/// Clue pulsing
float cluePhase = 0;
unsigned long nextPulseMillis = 0;
#define PulseGapMillis 100

void setup()
{
	Wire.begin(I2CAddress);
	// enable broadcasts to be received
	TWAR = (I2CAddress << 1) | 1;

  	Wire.onRequest(reportStatus);
	Wire.onReceive(receiveCommand);

	Serial.begin(9600);

	display.begin();
	display.control(2, 1); // intensity
	display.clear();
	display.update();

	state = STATE_CLUE;
	showClue();

	Serial.print("Maze module joining I2C bus with address: ");
	Serial.println(I2CAddress);
}

void loop()
{
	if(state == STATE_CLUE)
	{
		pulseClue();
	}
	else if(state == STATE_NAVIGATING)
	{
		showLocation();
	}

	if(!armed)
		return;

	if(deactivated)
		return;

}

void showClue() {
	display.clear();
	display.setPoint(clue1X, clue1Y, true);
	display.setPoint(clue2X, clue2Y, true);
	display.update();
}

void showLocation() {
	display.clear();
	display.setPoint(posX, posY);
	display.update();
}

void pulseClue() {
	if(nextPulseMillis > millis())
		return;

	nextPulseMillis = millis() + PulseGapMillis;

	cluePhase += 0.05;
	if(cluePhase > 2)
		cluePhase = 0;

	int clueIntensity = abs(sin(cluePhase * 3.142)) * 15.0/2;

	display.control(2, clueIntensity);
	display.update();
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
		
		// TODO: implement

		// else incorrect

		strikes++;
	}
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

	armed = true;
}

void explode()
{
	// TODO:
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
	else if(command == COMMAND_EXPLODE)
	{
		explode();
	}
}