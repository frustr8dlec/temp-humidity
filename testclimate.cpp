/* remotesensor.cpp : main project file.
   
   requires the following files to be included in a
   windows clr console application to work.

   sensor.h			Abstract class
   http.h			Http sensor class definition
   http.cpp			Http sensor class
   remotesensor.cpp	Main program

   http class contains contains .Net specific code to function 
   and therefore cannot be compiled in a linux environment.
   
   For raspberry Pi local functionality
   
   use
   
   am2320.h			AM2320 sensor class definition
   am2320.cpp		AM2320 sensor class
   
   Set 
   
   #define windows 0
    
   Compile with:

   g++ -std=c++11 am2320.cpp climate.hpp testclimate.cpp -o testclimate
   
*/

#include "stdafx.h" // Global includes options file

#define windows 0   // Set to 1 to compile for windows 0 for Rasp. Pi

#if windows != 1
#include <unistd.h>
#endif

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <typeinfo> 

#include "climate.hpp"

#define number_of_reads 6
#define number_of_seconds 60 * 60

#if windows == 1
using namespace System;  // Microsoft Specific
#endif

using namespace std;

void pause(int seconds){
#if windows == 1
	Threading::Thread::Sleep(TimeSpan::FromSeconds(seconds)); // Microsoft Specific Time Delay
#else
	sleep(seconds); //linux specific time delay
#endif	
}


int main()
{

	Climate roomB114;

	int sensorReadTime = 0;

	try { roomB114.clearSamples(); } catch (exception &e) { cerr << "Caught: " << e.what() << endl; }

	pause(1);
	for (int read_counter = 0; read_counter < number_of_reads; read_counter++)
	{
		cout << "Attempting to read sensor " << read_counter;
		try { 
			sensorReadTime = roomB114.readSensor(); 
			cout << fixed << setprecision(2);
			cout << " reading taken from sensor at : " << sensorReadTime << " seconds.";
			cout << " temperature " << roomB114.getTemperature(sensorReadTime) << " C";
			cout << " humidity " << roomB114.getHumidity(sensorReadTime) << "%" << endl;
			pause(1);
		}
		catch (exception &e) { 
			cout << " reaing failed." << endl;
			cerr << "Caught: " << e.what() << "Type: " << typeid(e).name() << endl;
		}
	}

	cout << "Number of samples taken in the last " << number_of_seconds << " seconds " << roomB114.sampleCount(number_of_seconds) << endl;
	cout << "Average Humidity in the last " << number_of_seconds << " seconds " << roomB114.averageHumidity(number_of_seconds) << endl;
	cout << "Minimum Humidity in the last " << number_of_seconds << " seconds " << roomB114.minimumHumidity(number_of_seconds) << endl;
	cout << "Maximum Humidity in the last " << number_of_seconds << " seconds " << roomB114.maximumHumidity(number_of_seconds) << endl;
	cout << "Average Temperature in the last " << number_of_seconds << " seconds " << roomB114.averageTemperature(number_of_seconds) << endl;
	cout << "Minimum Temperature in the last " << number_of_seconds << " seconds " << roomB114.minimumTemperature(number_of_seconds) << endl;
	cout << "Maximum Temperature in the last " << number_of_seconds << " seconds " << roomB114.maximumTemperature(number_of_seconds) << endl;
	cout << roomB114("humidity", 1) << ", " << roomB114("temperature", 1) << endl;
	return(0);
}

