/*
 Name:		espTestHarness.ino
 Created:	5/18/2018 12:57:56 PM
 Author:	bjf
*/

#include <HardwareSerial.h>
#include <wire.h>

//#define _SCAN_I2C

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

#ifdef 	_SCAN_I2C
	ScanI2C();
#endif


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


#ifdef _SCAN_I2C

void ScanI2C()
{
	{
		byte error, address;
		int nDevices;

		Serial.println("Scanning...");

		nDevices = 0;
		for (address = 1; address < 127; address++)
		{
			// The i2c_scanner uses the return value of
			// the Write.endTransmisstion to see if
			// a device did acknowledge to the address.
			Wire.beginTransmission(address);
			error = Wire.endTransmission();

			if (error == 0)
			{
				Serial.print("I2C device found at address 0x");
				if (address<16)
					Serial.print("0");
				Serial.print(address, HEX);
				Serial.println("  !");

				nDevices++;
			}
			else if (error == 4)
			{
				Serial.print("Unknown error at address 0x");
				if (address<16)
					Serial.print("0");
				Serial.println(address, HEX);
			}
		}
		if (nDevices == 0)
			Serial.println("No I2C devices found\n");
		else
			Serial.println("done\n");

		delay(5000);           // wait 5 seconds for next scan
	}
}


#endif
