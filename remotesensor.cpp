/* remotesensor.cpp : main project file.
   
   requires the following files to be included in a
   windows clr console application to work.

   sensor.h			Abstract class
   http.h			Http sensor class definition
   http.cpp			Http sensor class
   remotesensor.cpp	Main program

   http class contains contains .Net specific code to function 
   and therefore cannot be compiled in a linux environment.
   
*/
#include "stdafx.h" // Microsoft Specific

using namespace System;  // Microsoft Specific
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "http.h"


int main()
{
	Http sensor ;

	for (int read_counter = 0; read_counter < 5; read_counter++)
	{

		sensor.read_data();

		if (sensor.valid())
			std::cout << std::setprecision(2) << std::fixed
			<< "Humidity = " << sensor.get_humidity()
			<< "% Temp = " << sensor.get_temperature_in_c()
			<< "C Temp = " << sensor.get_temperature_in_f()
			<< "F" << std::endl;

		_sleep(1000);
		
	}

	system("pause");  // Microsoft Specific Code

	return(0);
}

