/*
 * generates a simple json file with the temp and humidity information
 * 
 * compile with g++ am2320.cpp json2file.cpp -o json2file
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
#include <fstream>
#include <iomanip>
#include <unistd.h>

// Catch CTRL + C etc.
#include <signal.h>

#include "am2320.h"






using namespace std;

ofstream json_file;

// Catch CTRL + C etc.
void my_handler(int s){
	
	printf("Caught signal %d\n",s);
	
	if(!json_file.is_open()) json_file.open("/var/www/html/data.json", ios::out | ios::trunc);
	

	json_file << "Content-Type: application/json\r\n\r\n";
	json_file << "{ \"Error\" : \"" << "Offline" << "\" "
			  << "}";
	json_file << "\n"; 
	
	json_file.close(); 
	         
    exit(1); 

}

int main( void )
{
	Am2320 sensor;
	
	
	// Catch CTRL + C etc.
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
	
	
	int error_counter;

	do {
		
		error_counter = 0;

		do {
			sensor.read_data();
			error_counter++;
		} while (!sensor.valid() && (error_counter < 5));
		
		
		json_file.open("/var/www/html/data.json", ios::out | ios::trunc);
		if(sensor.valid()) {
			
			json_file << "Content-Type: application/json\r\n\r\n";
			json_file << setprecision(2) << fixed 
				 << "{ \"Humidity\" : \"" << sensor.get_humidity() << "\", "
				 << "\"Centigrade\" : \"" << sensor.get_temperature_in_c() << "\", " 
				 << "\"Fahrenheit\" : \"" << sensor.get_temperature_in_f() << "\" " 
				 << "}";
			json_file << "\n";
		}
		else {
			json_file << "Content-Type: application/json\r\n\r\n";
			json_file << setprecision(2) << fixed 
				 << "{ \"Error\" : \"" << sensor.get_error() << "\" "
				 << "}";
			json_file << "\n";		
		}
		json_file.close();
		sleep(1);
		
	} while(1);
	
	return(0);
}
