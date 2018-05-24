
#include "dypUltraSonic.h"
#include <Arduino.h>
#include <HardwareSerial.h>

#define _MAX_DRY_READS	10

bool dypUltraSonic::readSensor()
{
	// scribble into the one NOT active
	distanceReadings *inactive = InactiveReadings();
	// flush the circQ
	m_sensorData.reset();
	inactive->reset();
	m_serial->flush();

	// fill a bucket ... we should check this for timeouts
	int countdown;
	for (countdown = 0; (m_serial->available() < m_sensorData.space()) && countdown < _MAX_DRY_READS; countdown++)
	{
		delay(100);
	}

	if (countdown == _MAX_DRY_READS)
	{
		// tell the cache we have failed

		return false;
	}


	while (m_serial->available() && m_sensorData.space())
	{
		m_sensorData.write(m_serial->read());
	}

	// we need at least 4 bytes
	bool syncFound = false;
	for (int examine = 0; !syncFound && (m_sensorData.available()>3); examine++)
	{

		if ((m_sensorData[0] == 0xff) && (((m_sensorData[0] + m_sensorData[1] + m_sensorData[2]) & 0xff) == m_sensorData[3]))
		{
			// we have a hit
			syncFound = true;
			// we'll fall out at the next loop condition check
		}
		else
		{
			// read past it
			m_sensorData.read();
			// and replace it
			m_sensorData.write(m_serial->read());
		}
	}
	if (syncFound)
	{
		for (int examine = 0; m_sensorData.available()>3; examine++)
		{
			// 0 is FF, 1 & 2 we want, # is a checksum
			/*unsigned char c1 = */m_sensorData.read();
			unsigned char c2 = m_sensorData.read();
			unsigned char c3 = m_sensorData.read();
			/*unsigned char c4 = */m_sensorData.read();
			int value = (c2 << 8) | (c3);
			inactive->write(value);
		}
		inactive->lock();
		
		SwapReadings();
		

		return true;
	}

	return false;
}

////////////////////////

bool TriggerUltrasonic::readSensor()
{
	distanceReadings *inactive = InactiveReadings();
	inactive->reset();

	// and start the clock
	for(int readings=0;readings<4;readings++)
	{
		// write to the trigger, for the proscribed time
		digitalWrite(m_trigger, HIGH);
		delayMicroseconds(m_triggerUS);
		digitalWrite(m_trigger, LOW);


		while (digitalRead(m_echo) == LOW);
		unsigned long start = micros();
		while (digitalRead(m_echo) == HIGH);
		unsigned long duration = micros() - start;
		float mms = (duration / m_echoDiv);

		inactive->write(mms);

		// and wait for echoes to vanish
		delay(m_triggerDelay);
	}

	inactive->lock();

	SwapReadings();


	return true;
}

void TriggerUltrasonic::begin()
{
	// set trig as output, echo as in
	pinMode(m_trigger, OUTPUT);
	digitalWrite(m_trigger, LOW);

	pinMode(m_echo, INPUT);
}