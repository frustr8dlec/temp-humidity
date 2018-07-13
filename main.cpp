/* Compile instructions
 * 
 * For AM2320 sensor: g++ am2320.cpp main.cpp -o AM2320 -lwiringPi -lwiringPiDev
 * For DHT11 sensor:  g++ dht.cpp main.cpp -o DHT11 -lwiringPi -lwiringPiDev
 * 
 */ 



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
