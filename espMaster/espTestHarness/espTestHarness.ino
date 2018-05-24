/*
 Name:		espTestHarness.ino
 Created:	5/18/2018 12:57:56 PM
 Author:	bjf
*/

#include <HardwareSerial.h>
#include <wire.h>

// the setup function runs once when you press reset or power the board
void setup() {

	delay(1000);
	Serial.begin(9600);
	Serial.println("Running ...");
	Wire.begin();
}

// the loop function runs over and over again until power down or reset
void loop() 
{


#define _OPTIMUM_READ	1960
#define _ALLOWED_RANGE	5
  
	int status = 0;
	uint8_t result = Wire.requestFrom(0x12, 3, status);
	if(result)
	{
		int read = (Wire.read()<<8)|Wire.read();
		if(abs(_OPTIMUM_READ -read)>_ALLOWED_RANGE)
		{
			Serial.printf("read %d from %d samples\n\r",read, Wire.read());
		}
		else
		{
			Serial.printf(".");
		}

	}
	else
	{
		Serial.printf("%d ",status);
	}

	
	delay(rand() % 5000);
}
