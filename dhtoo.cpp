#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAXTIMINGS	85
#define DHTPIN		7

int dht11_dat[5] = { 0, 0, 0, 0, 0 };

class  temp_humid_sensor {

        private:
		bool data_valid;
                float temperature;
                float humidity;
		int gpio_pin_number;
		int dht11_dat[5];

		void validate_sensor_data();

        public:
                temp_humid_sensor(int gpio_pin);

		void reset();
                void read();
		bool valid();

                float get_temperature_in_c();
                float get_temperature_in_f();
		float get_humidity();

        protected:

};

temp_humid_sensor::temp_humid_sensor(int gpio_pin) {
	this->gpio_pin_number = gpio_pin;
	this->reset();
}

void temp_humid_sensor::reset(){

	// Clear existing data
	this->dht11_dat[0] = this->dht11_dat[1] = this->dht11_dat[2] = this->dht11_dat[3] = this->dht11_dat[4] = 0;
	this->temperature = 0.0;
	this->humidity = 0.0;
	this->data_valid = false;

	// Send a reset signal
        pinMode( this->gpio_pin_number, OUTPUT );
        digitalWrite( this->gpio_pin_number, LOW );
        delay( 18 );
        digitalWrite( this->gpio_pin_number, HIGH );
        delayMicroseconds( 40 );

	// Enable reading on the GPIO pin for the sensor
        pinMode( this->gpio_pin_number, INPUT );
}

void temp_humid_sensor::validate_sensor_data(){
	this->data_valid = this->dht11_dat[4] == ( ( this->dht11_dat[0] + this->dht11_dat[1] + this->dht11_dat[2] + this->dht11_dat[3] ) & 0xFF );
}

bool temp_humid_sensor::valid(){
	return this->data_valid;
}

void temp_humid_sensor::read(){

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
                        this->dht11_dat[j / 8] <<= 1;
                        if ( counter > 55 )
                                this->dht11_dat[j / 8] |= 1;
                        j++;
                }
        }

        if ( j >= 40) {

		this->validate_sensor_data();

		if(this->data_valid) {
			this->humidity    = (float)this->dht11_dat[0] + (float)this->dht11_dat[1] / 100.0f;
			this->temperature = (float)this->dht11_dat[2] + (float)this->dht11_dat[3] / 100.0f;
		}
	}

}

float temp_humid_sensor::get_humidity(){
	return this->humidity;
}

float temp_humid_sensor::get_temperature_in_c(){
	return this->temperature;
}

float temp_humid_sensor::get_temperature_in_f(){
	return this->temperature;
}

class  temp_humid_data {

        private:
                float temperature_readings[10];
                float humidity_readings[10];

        public:
                temp_humid_data();
                float average_temperature();
                float average_humidity();
                float recent_temperature();
                float recent_humidity();
                float last_temperature();
		float last_humidity();

        protected:

};

int main( void )
{

	if ( wiringPiSetup() == -1 )
		exit( 1 );

	temp_humid_sensor sensor( DHTPIN );

	for ( int read_counter = 0; read_counter < 10; read_counter++ )
	{

		sensor.read();
		if(sensor.valid())
                	printf( "Humidity = %f %% Temperature = %f C \n", sensor.get_humidity(), sensor.get_temperature_in_c());

		delay( 1000 );
	}

	return(0);
}
