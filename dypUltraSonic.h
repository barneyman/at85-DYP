#include <softwareSerial.h>	// using espsoftwareserial for esp

#include <circQ.h>

class dypReadings : public circQueueT<32, 4, int>
{
public:
	dypReadings()
	{
	}

	qStorageType average, min, max, median;

	void lock()
	{
		
		// walk thru them, sorting min to max
		for(bool sorted=false;!sorted;)
		{
			sorted = true;
			// 
			average = peek(available()-1);
			// 
			for (unsigned each = 0; each < available()-1; each++)
			{

				if (peek(each) > peek(each + 1))
				{
					// swap them
					swap(each, each + 1);
					sorted = false;
				}
				else
				{
					average += peek(each);
					average /= 2;
				}
			}

			min = peek(0);
			max = peek(available() - 1);

			if (available() > 2)
				median = peek((available() / 2) - 1);
			else
				median = peek(0);
		}
	}

protected:

	void swap(int a, int b)
	{
		qCounterType offsetA = readCursor + a, offsetB = readCursor+b;
		qStorageType scratch= m_data[offsetA];
		m_data[offsetA]= m_data[offsetB];
		m_data[offsetB]=scratch;

	}

};

class dypUltraSonic
{
public:
	dypUltraSonic(int rxPin, int txPin) :m_serial(rxPin,txPin)
	{
	}

	void begin()
	{
		m_serial.begin(9600);
	}

	bool readSensor();

protected:

	SoftwareSerial m_serial;
	circQueueT<32, 4> m_sensorData;
	dypReadings m_sensorReadings;

};


