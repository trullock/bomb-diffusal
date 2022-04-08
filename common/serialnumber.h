#ifndef SerialNumber_h
#define SerialNumber_h

char serialCharToAscii(uint8_t value)
{
	// 0-9
	if(value >=0 && value <= 9)
		return 48 + value;
	
	// A-Z
	if(value > 9)
		return 65 + value - 10;

	return 0;
}


void generateSerialNumber(byte (&serialNumber)[5])
{
	// TODO: ensure we generate a S/N which the combinations module can turn into useful combinations
	for(byte i = 0; i < sizeof(serialNumber); i++)
		serialNumber[i] = random(0, 36);
}


#endif