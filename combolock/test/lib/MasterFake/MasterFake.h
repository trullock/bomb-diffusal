#ifndef MasterFake_h
#define MasterFake_h

#include <constants.h>
#include <stdint.h>
#include <Wire.h>
#include <unity.h>
#include <ArduinoFake.h>


void Master_SendCommand_SetSerialNumber()
{
	srand(time(NULL));
	uint8_t a = rand() % 36;
	uint8_t b = rand() % 36;
	uint8_t c = rand() % 36;
	uint8_t d = rand() % 36;
	uint8_t e = rand() % 36;

	Wire.test_Receive(COMMAND_SERIAL, a, b, c, d, e);
}

void Master_SendCommand_SetSerialNumber(char a, char b, char c, char d, char e)
{
	a -= a >= 'A' && a <= 'Z' ? 65 : 48;
	b -= b >= 'A' && b <= 'Z' ? 65 : 48;
	c -= c >= 'A' && c <= 'Z' ? 65 : 48;
	d -= d >= 'A' && d <= 'Z' ? 65 : 48;
	e -= e >= 'A' && e <= 'Z' ? 65 : 48;

	Wire.test_Receive(COMMAND_SERIAL, a, b, c, d, e);
}

void Master_SendCommand_SetSerialNumber(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e)
{
	Wire.test_Receive(COMMAND_SERIAL, a, b, c, d, e);
}


void Master_SendCommand_Arm()
{
	Wire.test_Receive(COMMAND_ARM);
}

void Master_SendCommand_SetDifficulty(uint8_t diff)
{
	Wire.test_Receive(COMMAND_DIFFICULTY, diff);
}

void Master_Verify_WasInterrupted(uint8_t pin)
{
	//Verify(Method(ArduinoFake(), digitalWrite).Using(pin, HIGH)).Once();
	//Verify(Method(ArduinoFake(), digitalWrite).Using(pin, LOW)).Once();
}

void Master_Verify_DidReceiveStrike(uint8_t pin)
{
	Master_Verify_WasInterrupted(pin);

	Wire.test_Request();
	byte state = Wire.writeBuffer[0];

	byte strike = Wire.writeBuffer[1];
	TEST_ASSERT_EQUAL(1, strike);
}


void Master_Verify_DidReceiveDeactivation(uint8_t pin, uint8_t strikes)
{
	Master_Verify_WasInterrupted(pin);

	Wire.test_Request();
	byte state = Wire.writeBuffer[0];
	TEST_ASSERT_EQUAL(STATE_DEACTIVATED, state);

	byte strike = Wire.writeBuffer[1];
	TEST_ASSERT_EQUAL(strikes, strike);
}

#endif