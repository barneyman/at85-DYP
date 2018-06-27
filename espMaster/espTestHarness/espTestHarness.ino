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


#define _OPTIMUM_READ	1793
#define _ALLOWED_RANGE	5
  
	int status = 0;
	Wire.beginTransmission(0x12);
	Wire.write(0);
	Wire.endTransmission();
	delay(500);
	uint8_t result = Wire.requestFrom(0x12, 4, status);
	if(result)
	{
		int readState = Wire.read();

		if (readState == 1)
		{

			int read = (Wire.read() << 8) | Wire.read();
			if (abs(_OPTIMUM_READ - read) > _ALLOWED_RANGE)
			{
				Serial.printf("read %d from %d samples\n\r", read, Wire.read());
			}
			else
			{
				Serial.printf(".");
			}
		}
		else
		{
			Serial.printf("read failed %d\n\r", readState);
			while (Wire.available())
				Wire.read();
		}

	}
	else
	{
		Serial.printf("%d ",status);
	}

	
	delay((rand() % 15000)+500);
}
