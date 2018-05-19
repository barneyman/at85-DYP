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

	static int lastRead = 0;
#define _OPTIMUM_READ	1920
  
	if(Wire.requestFrom(0x12, 3))
	{
		int read = (Wire.read()<<8)|Wire.read();
		if(abs(_OPTIMUM_READ -read)>5)
		{
			Serial.printf("read %d from %d samples\n\r",read, Wire.read());
			lastRead = read;
		}
		else
		{
			Serial.printf(".");
		}

	}
	else
	{
		Serial.println("failed");
	}

	delay(750);
}
