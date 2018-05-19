
#ifdef __AVR_ATtiny85__
#include <SoftwareSerialTinyWIre.h>	
#include <TinyWire.h>
#else
#include <softwareSerial.h>	// using espsoftwareserial for esp
#endif

#include "dypUltraSonic.h"

#ifdef __AVR_ATtiny85__
#define _RX_PIN		PB3
#define _TX_PIN		PB1
#define LED_BUILTIN	PB4
#else
#define _RX_PIN		D5
#define _TX_PIN		D6
#endif

#ifdef __AVR_ATtiny85__
SoftwareSerialTinyWIre softy(_RX_PIN, _TX_PIN);
TinyTwi tw(&softy);
#else
SoftwareSerial softy(_RX_PIN, _TX_PIN);
#endif

dypUltraSonic dyp(&softy);


void setup()
{
	delay(200);
	/* add setup code here */
#ifdef __AVR_ATtiny85__

	tw.onReceive(i2cDataReceived);
	tw.onRequest(i2cDataRequested);
	tw.begin(0x12);

#else

	Serial.begin(9600);
	Serial.println("Running ...");
#endif
	pinMode(LED_BUILTIN, OUTPUT);

	softy.begin(9600);
	dyp.begin();

}

#ifdef __AVR_ATtiny85__

void i2cDataReceived(int count)
{
}

void i2cDataRequested(void)
{
	// just hand back the mean
	uint16_t number = dyp.m_sensorReadings.median;
	tw.send(number >> 8);
	tw.send(number & 0xff);
	tw.send(dyp.m_sensorReadings.available()&0xff);
}

#endif

bool flipflop = true;

#define _OPTIMUM_READING 1920

void loop()
{
	if(dyp.readSensor())
	{
		digitalWrite(LED_BUILTIN, flipflop ? LOW : HIGH);
		flipflop = !flipflop;

#ifndef __AVR_ATtiny85__
		if (abs(dyp.m_sensorReadings.average - _OPTIMUM_READING) > 5)
		{
			Serial.printf("ave %d from %d samples\n\r", dyp.m_sensorReadings.average, dyp.m_sensorReadings.available());
			while (dyp.m_sensorReadings.available())
			{
				Serial.printf("%d ", dyp.m_sensorReadings.read());
			}
			Serial.println("\n\r=================");
		}
#endif
	}
	else
	{
#ifndef __AVR_ATtiny85__
		Serial.println("NO DATA");
#endif
	}

#ifndef __AVR_ATtiny85__
	delay(1000);
#endif
}

