/*
 * generates a simple html page with the temp and humidity information
 * 
 * compile with g++ am2320.cpp htmlcgi.cpp -o temphtml.cgi
 * 
 * apache2 install on a raspberry pi required
 * 
 * sudo apt-get install apache2
 * sudo a2enmod cgi
 * 
 * after compilation (makes a security hole):
 * 
 * sudo cp temphtml.cgi /usr/lib/cgi-bin
 * cd /usr/lib/cgi-bin
 * sudo chown root.pi temphtml.cgi
 * sudo chmod 4555 temphtml.cgi
 * 
 * access with:
 * 
 * http://localhost/cgi-bin/temphtml.cgi
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
	cout << "Content-type:text/html\r\n\r\n";
	cout << "<html>\n";
	cout << "<head>\n";
	cout << "<title>Remote Temperature and Humidity Info.</title>\n";
	cout << "</head>\n";
	cout << "<body>\n";
	cout << "<h2>Temp and Humidity Data</h2>\n";
	cout << "<p>";
	cout << setprecision(2) << fixed 
		 << "Humidity = " << sensor.get_humidity() 
		 << "% Temp = " << sensor.get_temperature_in_c() 
		 << "C Temp = " << sensor.get_temperature_in_f() 
		 << "F";
	cout << "</p>\n";
	cout << "</body>\n";
	cout << "</html>\n";
	}
	else {
	cout << "Content-type:text/html\r\n\r\n";
	cout << "<html>\n";
	cout << "<head>\n";
	cout << "<title>Error Status</title>\n";
	cout << "</head>\n";
	cout << "<body>\n";
	cout << "<h2>Error</h2>\n";
	cout << "<p>";
	cout << "Error reading sensor : " << sensor.get_error();
	cout << "</p>\n";
	cout << "</body>\n";
	cout << "</html>\n";		
	}
	
	return(0);
}
