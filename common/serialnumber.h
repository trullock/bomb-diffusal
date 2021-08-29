#ifndef SerialNumber_h
#define SerialNumber_h

char serialCharToAscii(uint8_t value)
{
	// A-Z
	if(value >=0 && value <= 25)
		return 65 + value;
	
	// 0-9
	if(value > 25)
		return 48 + value - 26;

	return 0;
}

#endif