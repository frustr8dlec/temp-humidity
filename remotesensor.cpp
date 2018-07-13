// remotesensor.cpp : main project file.

#include "stdafx.h"

using namespace System;
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "http.h"


int main(array<System::String ^> ^args)
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

	system("pause");
	return(0);
}

