#include "serialnumber.h"

#ifndef Lock_h
#define Lock_h

void setCombination(byte (&combination)[5], byte (&serialNumber)[5])
{
	/* Combolock rules: 
						first number is sum of all digits in the sn,
						second number is number of consonants,
						third number is letter index (A = 1, Z = 26) of first vowel
						fortuh number is the sum of the letter indicies (A = 1, Z = 26) of all letters
						fifth number is the sum of all the SN characters (A = 1, Z = 26) + 0-9
	*/

	byte 	integerSum = 0,
			consonantCount = 0,
			firstVowelIndex = -1,
			characterIndexSum = 0,
			characterSum = 0;

	for(byte i = 0; i < 5; i++)
	{
		char c = serialCharToAscii(serialNumber[i]);
		
		// number
		if(c >= '0' && c <= '9')
		{
			integerSum += c - '0';
			characterSum += c - '0';
		}
		// consonant
		else if(c != 'A' && c != 'E' && c != 'I' && c != 'O' && c != 'U')
		{
			consonantCount++;
			characterSum += c - 'A' + 1;
			characterIndexSum += i + 1;
		}
		// else its a vowel
		else if(firstVowelIndex == -1)
		{
			firstVowelIndex = i + 1;
			characterIndexSum += i + 1;
		}
	}

	// TODO: need to prevent neighbouring numbers from ever being the same as each other
	combination[0] = integerSum;
	combination[1] = consonantCount;
	combination[2] = firstVowelIndex;
	combination[3] = characterIndexSum;
	combination[4] = characterSum;
}

#endif