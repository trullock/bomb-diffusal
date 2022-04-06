
#include "Wire.h"
#include <cstring>

void (*Wire_::user_onRequest)(void);

void (*Wire_::user_onReceive)(int);

bool Wire_::begin(uint8_t address) { return true; }

void Wire_::onReceive( void (*function)(int) ) {
	user_onReceive = function;
}

void Wire_::onRequest( void (*function)(void) ) {
	user_onRequest = function;
}

uint8_t Wire_::read(){
	if(readBufferLength > 0)
	{
		if(readBufferReadIndex > readBufferLength - 1)
			return 255;

		return readBuffer[readBufferReadIndex++];
	}

	return 255;
}

void Wire_::write(uint8_t b) { 
	writeBuffer[writeBufferLength++] = b;
}

void Wire_::test_Request()
{
	user_onRequest();
}

void Wire_::test_Receive(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4, uint8_t arg5)
{
	memset(readBuffer, 0, sizeof(readBuffer));

	readBufferLength = 0;
	readBuffer[readBufferLength++] = arg0;
	if(arg1 != 255)
		readBuffer[readBufferLength++] = arg1;
	if(arg2 != 255)
		readBuffer[readBufferLength++] = arg2;
	if(arg3 != 255)
		readBuffer[readBufferLength++] = arg3;
	if(arg4 != 255)
		readBuffer[readBufferLength++] = arg4;
	if(arg5 != 255)
		readBuffer[readBufferLength++] = arg5;

	readBufferReadIndex = 0;
	
	user_onReceive(readBufferLength);

	// TODO: reset here?
}

Wire_ Wire;

// mock register
uint8_t TWAR = 0;
