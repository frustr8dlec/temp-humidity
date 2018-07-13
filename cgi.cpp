#include <wiringPi.h>
#include <iostream>
#include <iomanip>

#include "am2320.h"
//#include "dht.h"

int main( void )
{
    wiringPiSetup();

	Am2320 sensor;
//	Dht11 sensor;
	
	for ( int read_counter = 0; read_counter < 5; read_counter++ )
	{

		sensor.read_data();
		
		if(sensor.valid())
            std::cout 	<< std::setprecision(2) << std::fixed 
						<< "Humidity = " << sensor.get_humidity() 
						<< "% Temp = " << sensor.get_temperature_in_c() 
						<< "C Temp = " << sensor.get_temperature_in_f() 
						<< "F" << std::endl;

		delay(1000); 
		
	}

	return(0);
}
