
#include "dypUltraSonic.h"
#include <Arduino.h>
#include <HardwareSerial.h>


bool dypUltraSonic::readSensor()
{
	// flush it
	m_sensorData.reset();
	m_sensorReadings.reset();
	m_serial->flush();

	// fill a bucket ... we shold check this for timeouts
	while (m_serial->available() < m_sensorData.space())
	{
		delay(100);
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
			// we'll fall out at te next loop condition check
		}
		else
		{
			// read past it
			m_sensorData.read();
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
			m_sensorReadings.write(value);
		}

		m_sensorReadings.lock();

		return true;
	}

	return false;
}