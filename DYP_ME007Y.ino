#include <softwareSerial.h>	// using espsoftwareserial for esp
#include <circQ.h>

#define _RX_PIN		D5
#define _TX_PIN		D6


SoftwareSerial dyp(_RX_PIN, _TX_PIN);

void setup()
{
	delay(2000);
	/* add setup code here */
	Serial.begin(9600);
	Serial.println("Running ...");

	dyp.begin(9600);

}

circQueueT<32, 4> sensorData;


void loop()
{
	readSensor();


	delay(2000);
}


bool readSensor()
{
	// flush it
	sensorData.reset();
	// this should check or it will wdt
	while (dyp.available() < sensorData.space())
	{
		Serial.printf("#");
		delay(100);
	}

	while (dyp.available() && sensorData.space())
	{
		sensorData.write(dyp.read());
	}

	// we need at least 4 bytes
	bool syncFound = false;
	for (int examine = 0; !syncFound && (sensorData.available()>3); examine++)
	{

		if ((sensorData.peek(0)==0xff) && (((sensorData.peek(0) + sensorData.peek(1) + sensorData.peek(2)) & 0xff) == sensorData.peek(3)))
		{
			// we have a hit
			syncFound = true;
			// we'll fall out at te next loop condition check
		}
		else
		{
			// read past it
			sensorData.read();
		}
	}
	if (syncFound)
	{
		for (int examine = 0; sensorData.available()>3; examine ++)
		{
			// 0 is FF, 1 & 2 we want, # is a checksum
			byte c1 = sensorData.read();
			byte c2 = sensorData.read();
			byte c3 = sensorData.read();
			byte c4 = sensorData.read();
			int value = (c2 << 8) | (c3);
			Serial.printf("%02x %02x %02x %02x - sensor %d - avail %d \n\r",c1,c2,c3,c4,value, sensorData.available());
		}
		Serial.println("\n\r----------------------------");
		return true;
	}

	Serial.println(":");

	return false;
}