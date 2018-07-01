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
	pinMode(LED_BUILTIN, OUTPUT);
}


#include <atultra.h>
ATultrasonic ats;

// the loop function runs over and over again until power down or reset
void loop() 
{


#define _OPTIMUM_READ	1793
#define _ALLOWED_RANGE	5

	int mms, numreads, readstate;
	digitalWrite(LED_BUILTIN, LOW);

	bool res = ats.GetReading(mms, numreads, readstate);

	digitalWrite(LED_BUILTIN, HIGH);


	if (res)
	{
		if (abs(_OPTIMUM_READ - mms) > _ALLOWED_RANGE)
		{
			Serial.printf("read %d from %d samples\n\r", mms, numreads);
		}
		else
		{
			Serial.printf(".");
		}

	}
	else
	{
		Serial.printf("read failed %d\n\r", readstate);
	}

  

	delay(30 * 1000);
	//delay((rand() % 15000)+500);
}
