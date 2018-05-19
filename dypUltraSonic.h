#include <Stream.h>
#include <circQ.h>

class dypReadings : public circQueueT<8, 3, int>
{
public:
	dypReadings()
	{
	}

	volatile qStorageType average, min, max, median;

	virtual void reset()
	{
		average= min= max= median = 0;
		circQueueT<8, 3, int>::reset();
	}

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
			{
				if (available() % 2)
				{
					median = (available() >> 1) + 1;
				}
				else
					median = peek(available() >> 1);
			}
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
	dypUltraSonic(Stream*serialPort) :m_serial(serialPort)
	{
	}

	void begin()
	{
		
	}

	bool readSensor();

	dypReadings m_sensorReadings;

protected:

	Stream* m_serial;
	circQueueT<32, 5> m_sensorData;

};


