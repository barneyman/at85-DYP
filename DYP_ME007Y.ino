#include <softwareSerial.h>	// using espsoftwareserial for esp
#include <circQ.h>

#define _RX_PIN		D5
#define _TX_PIN		D6


SoftwareSerial dyp(_RX_PIN,_TX_PIN);

void setup()
{
	delay(2000);
  /* add setup code here */
	Serial.begin(9600);
	Serial.println("Running ...");

	dyp.begin(9600);

}

#define MAX_GRAB	32

struct 
{
	unsigned writeCursor, size;
	byte buffer[MAX_GRAB];
	unsigned calculated[MAX_GRAB];
} serialData = { 0, MAX_GRAB };


circQueueT<32, 4> blah;


void loop()
{
	readSensor();


	delay(500);
}


bool readSensor()
{
	// this should check or it will wdt
	while (dyp.available() < serialData.size)
	{
		Serial.printf(".");
		delay(100);
	}

	// read as much as we can
	int canBeRead = serialData.size - serialData.writeCursor;
	int actualRead = dyp.readBytes(&serialData.buffer[serialData.writeCursor], canBeRead);

	serialData.writeCursor += actualRead;
	// we need at least 4 bytes
	if (serialData.writeCursor > 3)
	{
		int byteStep = 1;
		int readCursor = 0;
		for (int examine = 0; examine < serialData.writeCursor - 3; examine += byteStep)
		{
			// chksum test
			if (((serialData.buffer[examine] + serialData.buffer[examine + 1] + serialData.buffer[examine + 2]) & 0xff) == serialData.buffer[examine + 3])
			{
				// woot - locked
				byteStep = 4;
				readCursor += 4;

				int value = ((serialData.buffer[examine + 1]) << 8) | (serialData.buffer[examine + 2]);

				Serial.printf("%3d. [ %02x %02x %02x %02x ] - %d\n\r", examine, serialData.buffer[examine], serialData.buffer[examine + 1], serialData.buffer[examine + 2], serialData.buffer[examine + 3], value);
			}
			else
			{
				// boo
				byteStep = 1;
				readCursor += 1;

				Serial.println("!");
			}
		}
		memmove(&serialData.buffer[0], &serialData.buffer[readCursor], serialData.size - readCursor);
		serialData.writeCursor -= readCursor;
	}

	Serial.println(":");


}