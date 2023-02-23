#include <ShiftRegister74HC595.h>
#ifndef whmxe_595_h
#define whmxe_595_h

class whmxe_595
{
	ShiftRegister74HC595 display;
	const uint8_t segments[10] = {
		B11000000, // 0
		B11111001, // 1 
		B10100100, // 2
		B10110000, // 3 
		B10011001, // 4
		B10010010, // 5
		B10000011, // 6
		B11111000, // 7
		B10000000, // 8
		B10011000  // 9
	};

public:
	whmxe_595(int serialDataPin, int clockPin, int latchPin)
		: display(2, serialDataPin, clockPin, latchPin)
	{
		this->clear();
	}

	void setNumber(uint8_t number)
	{
		int tens = number / 10;
		int ones = number % 10;

		uint8_t values[]= {segments[ones], segments[tens]};
		display.setAll(values);	
	}

	void clear()
	{
		display.setAllHigh(); // inverted
	}
};

#endif