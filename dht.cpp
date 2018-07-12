#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "dht.h"

Dht11::Dht11() {
	this->gpio_pin_number = DEFAULTDHTPIN;
	this->sensor_status = 0;
	this->reset();

}

Dht11::Dht11(int gpio_pin) {
	this->gpio_pin_number = gpio_pin;
	this->sensor_status = 0;
	this->reset();
}

void Dht11::reset(){

	// Clear existing data
	this->data[0] = this->data[1] = this->data[2] = this->data[3] = this->data[4] = 0;
	this->temperature = 0.0;
	this->humidity = 0.0;
	this->data_valid = false;

	// Send a reset signal
	pinMode( this->gpio_pin_number, OUTPUT );
	digitalWrite( this->gpio_pin_number, LOW );
	delay( 20 );
	digitalWrite( this->gpio_pin_number, HIGH );
	delayMicroseconds( 40 );

	// Enable reading on the GPIO pin for the sensor
	pinMode( this->gpio_pin_number, INPUT );
}

void Dht11::validate_sensor_data(){
	
	this->data_valid = this->data[4] == ( ( this->data[0] + this->data[1] + this->data[2] + this->data[3] ) & 0xFF );
	
	if(data_valid) return;
	
	this->sensor_status = 10;
		
}


void Dht11::read_data(){

	uint8_t laststate       = HIGH;
	uint8_t counter         = 0;
	uint8_t j               = 0, i;

	this->reset();

	for ( i = 0; i < MAXTIMINGS; i++ )
	{
			counter = 0;
			while ( digitalRead( this->gpio_pin_number ) == laststate )
			{
					counter++;
					delayMicroseconds( 1 );
					if ( counter == 255 ) break;
			}
			
			laststate = digitalRead( this->gpio_pin_number );

			if ( counter == 255 ) break;

			if ( (i >= 4) && (i % 2 == 0) )
			{
					this->data[j / 8] <<= 1;
					if ( counter > 50 )
							this->data[j / 8] |= 1;
					j++;
			}
	}

	if ( j >= 40) {

		this->validate_sensor_data();

		if(this->data_valid) {
			this->humidity    = (float)this->data[0] + (float)this->data[1] / 100.0f;
			this->temperature = (float)this->data[2] + (float)this->data[3] / 100.0f;
		}
	}
}
