
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
	if(bufferLength > 0)
	{
		if(readIndex > bufferLength - 1)
			return 255;

		return buffer[readIndex++];
	}

	return 255;
}

void Wire_::write(uint8_t b) { 

}

void Wire_::test_Receive(uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4)
{
	memset(buffer, 0, sizeof(buffer));

	bufferLength = 0;
	buffer[bufferLength++] = arg0;
	if(arg1 != 255)
		buffer[bufferLength++] = arg1;
	if(arg2 != 255)
		buffer[bufferLength++] = arg2;
	if(arg3 != 255)
		buffer[bufferLength++] = arg3;
	if(arg4 != 255)
		buffer[bufferLength++] = arg4;

	readIndex = 0;
	
	user_onReceive(bufferLength);
}

Wire_ Wire;

// mock register
uint8_t TWAR = 0;
