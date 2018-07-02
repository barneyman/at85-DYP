
#ifdef __AVR_ATtiny85__
#include <SoftwareSerialTinyWIre.h>	
#include <TinyWire.h>
#include <avr\sleep.h>

#define _SLEEP_BETWEEN

#else
#include <softwareSerial.h>	// using espsoftwareserial for esp
#endif




// using the Serial TX version
#define _DYP_TX
// a ping / echo variant
//#define _DYP_PWM


#ifdef _DYP_TX
#include <dypUltraSonic.h>
#else
#include <trigecho.h>
#endif

#ifdef __AVR_ATtiny85__

#ifdef _DYP_TX
	#define _RX_PIN		PB3
	#define _TX_PIN		PB1
#endif
#ifdef _DYP_PWM
#define _TRIGGER_PIN		PB3
#define _ECHO_PIN			PB1
#endif


#define LED_BUILTIN	PB4

#else

#ifdef _DYP_TX
	#define _RX_PIN		D5
	#define _TX_PIN		D6
#endif

#ifdef _DYP_PWM
	#define _TRIGGER_PIN		D5
	#define _ECHO_PIN			D6
#endif


#endif



#ifdef __AVR_ATtiny85__



#ifdef _DYP_TX
SoftwareSerialTinyWIre softy(_RX_PIN, _TX_PIN);
TinyTwi tw(&softy);
#else
TinyTwi tw;
#endif


#else
#ifdef _DYP_TX
SoftwareSerial softy(_RX_PIN, _TX_PIN);
#endif
#endif

// no pin
//#define XISTIRONPIN	NO_XSISTOR

#ifdef __AVR_ATtiny85__
#define XISTIRONPIN	PCINT1
#else
#define XISTIRONPIN	D7
#endif

#ifdef _DYP_TX
dypUltraSonic sensor(&softy, XISTIRONPIN);
#endif

#ifdef _DYP_PWM
HRS04 sensor(_TRIGGER_PIN,_ECHO_PIN);
#endif

//#define _OPTIMUM_READING 1798
#define _OPTIMUM_READING_VARIANCE	5

void setup()
{
	/* add setup code here */
#ifdef LED_BUILTIN
	pinMode(LED_BUILTIN, OUTPUT);
#endif

#ifdef __AVR_ATtiny85__
	
	ADCSRA &= ~(1 << ADEN); //Disable ADC, saves ~230uA

#ifdef _SLEEP_BETWEEN
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
#endif

	tw.onReceive(i2cDataReceived);
	tw.onRequest(i2cDataRequested);
	tw.begin(0x12);

#else

	Serial.begin(9600);
	Serial.println("Sensor Running ...");

#ifdef _OPTIMUM_READING
	Serial.printf("only values different from %d will be output\n\r", _OPTIMUM_READING);
#endif

#endif

#ifdef _DYP_TX
	softy.begin(9600);
#endif

	sensor.begin();

}


#ifdef __AVR_ATtiny85__

volatile bool doReading = false;

// to flag we can sleep
volatile bool readingRetrieved = true;


// this is an ISR - so we can do very little except raise a flag!
void i2cDataReceived(int count)
{

	while (TinyWire.available())
		TinyWire.read();
	// force a reading
	doReading = true;
#ifdef LED_BUILTIN
	PORTB |= 1 << LED_BUILTIN;
#endif
	readingRetrieved = false;
}

void i2cDataRequested(void)
{
	tw.send(sensor.LastReadState());
	// just hand back the mean
	uint16_t number = sensor.Readings()->median;
	tw.send((number >> 8)&0xff);
	tw.send(number & 0xff);
	tw.send(sensor.Readings()->available()&0xff);
	readingRetrieved = true;
#ifdef LED_BUILTIN
	PORTB &= ~(1 << LED_BUILTIN);
#endif
}

#endif

unsigned int minSeen = -1, maxSeen=0;

unsigned long lastAwake = 0;
#define _MAX_WAKE_TIME	10000

bool flipflop = false; 

void loop()
{


#ifdef __AVR_ATtiny85__
	if (doReading)
	{
		// don't come thru here again
		doReading = false;

		// give i2c time to settle
		delay(50);
#endif
		if (sensor.readSensor())
		{

#ifndef __AVR_ATtiny85__

#ifdef _OPTIMUM_READING
			if (abs(sensor.Readings()->average - _OPTIMUM_READING) > _OPTIMUM_READING_VARIANCE)
#endif
			{
				if (sensor.Readings()->median > maxSeen)
					maxSeen = sensor.Readings()->median;

				if (sensor.Readings()->median < minSeen)
					minSeen = sensor.Readings()->median;

				Serial.printf("median %d from %d samples\n\r", sensor.Readings()->median, sensor.Readings()->available());
				while (sensor.Readings()->available() > 0)
				{
					Serial.printf("%d ", sensor.Readings()->read());
				}
				Serial.printf("\n\rmax %u min %u", maxSeen, minSeen);
				Serial.println("\n\r=================");
			}
		}
		else
		{
			Serial.println("NO DATA");
#endif
		}

#ifdef __AVR_ATtiny85__
	}

#ifdef _SLEEP_BETWEEN
	if ((millis()- lastAwake)>_MAX_WAKE_TIME)
	{
#ifdef LED_BUILTIN		
		PORTB &= ~(1 << LED_BUILTIN);
#endif
		flipflop = !flipflop;
		set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
		sleep_mode();                        // System actually sleeps here
		lastAwake = millis();
#ifdef LED_BUILTIN
		PORTB |= 1 << LED_BUILTIN;
#endif
	}
#endif

#else
	delay(5000);
#endif
}

