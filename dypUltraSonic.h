#include <Stream.h>
#include "ultrasonic.h"

class dypUltraSonic : public readingsHandler
{
public:
	dypUltraSonic(Stream*serialPort) :m_serial(serialPort)
	{
	}

	void begin()
	{
	}

	bool readSensor();


protected:


	Stream* m_serial;
	circQueueT<32, 5> m_sensorData;
};




