#include "dypUltraSonic.h"

#define _RX_PIN		D5
#define _TX_PIN		D6




dypUltraSonic dyp(_RX_PIN, _TX_PIN);


void setup()
{
	delay(2000);
	/* add setup code here */
	Serial.begin(9600);
	Serial.println("Running ...");

	dyp.begin();

}



void loop()
{



	dyp.readSensor();


	delay(5000);
}

