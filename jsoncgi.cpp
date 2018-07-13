/*
 * generates a simple json file with the temp and humidity information
 * 
 * compile with g++ am2320.cpp jsoncgi.cpp -o tempjson.cgi
 * 
 * apache2 install on a raspberry pi required
 * 
 * sudo apt-get install apache2
 * sudo a2enmod cgi
 * 
 * after compilation (makes a security hole):
 * 
  sudo cp tempjson.cgi /usr/lib/cgi-bin
  cd /usr/lib/cgi-bin
  sudo chown root.pi tempjson.cgi
  sudo chmod 4555 tempjson.cgi
 * 
 * access with:
 * 
 * http://localhost/cgi-bin/tempjson.cgi
 * 
 */

#include <iostream>
#include <iomanip>

#include "am2320.h"

using namespace std;

int main( void )
{
	Am2320 sensor;
	
	int error_counter = 0;
	
	do {
		sensor.read_data();
		error_counter++;
	} while (!sensor.valid() && (error_counter < 5));
	
	if(sensor.valid()) {
		cout << "Content-Type: application/json\r\n\r\n";
		cout << setprecision(2) << fixed 
			 << "{ \"Humidity\" : \"" << sensor.get_humidity() << "\", "
			 << "\"Centigrade\" : \"" << sensor.get_temperature_in_c() << "\", " 
			 << "\"Fahrenheit\" : \"" << sensor.get_temperature_in_f() << "\" " 
			 << "}";
		cout << "\n";
	}
	else {
		cout << "Content-Type: application/json\r\n\r\n";
		cout << setprecision(2) << fixed 
			 << "{ \"Error\" : \"" << sensor.get_error() << "\" "
			 << "}";
		cout << "\n";		
	}
	
	return(0);
}
