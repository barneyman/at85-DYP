
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
	delay(2000);
	/* add setup code here */
#ifdef __AVR_ATtiny85__

	tw.onReceive(i2cDataReceived);
	tw.onRequest(i2cDataRequested);
	tw.begin(0x12);

	pinMode(LED_BUILTIN, OUTPUT);

#else
	Serial.begin(9600);
	Serial.println("Running ...");
#endif

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
	tw.send(
		dyp.m_sensorReadings.average / 10
	);
}

#endif

bool flipflop = true;

void loop()
{
	dyp.readSensor();
	digitalWrite(LED_BUILTIN, flipflop ? LOW : HIGH);
	flipflop = !flipflop;
	delay(1000);
}

