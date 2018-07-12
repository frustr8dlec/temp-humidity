/* g++ -o am2320 am2320.c
 * 
 * Reads an AM2320 or AM2321 usIng I2C bus on the raspberry PI
 * I2C has to be enabled with sudo raspi-config to work.
 * 
 * Connections 
 * 
 *   AM 2320
 *  *********
 *  *********
 *  *********
 *  *********
 *  *********
 *   * * * *
 *   * * * *
 *   * * * *
 *   V S G S
 *   D D N C
 *   D A D L
 * 
 *   R B B W
 *   E R L H
 *   D N K T
 * 
 *  Raspbery PI
 *        
 *  2 o o o o o o o o o o o o o o o o o o o o 40
 *  1 x x x o x o o o o o o o o o o o o o o o 39
 *    V S S   G
 *    D D C   N
 *    D A L   D
 * 
*/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h> 
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>

#define I2C_DEVICE "/dev/i2c-1"
#define AM2321_ADDR 0x5C


class  temp_humid_sensor {

	private:
	
		bool data_valid;
		
		float temperature;
		float humidity;
			
		int sensor_status;
		int fd;
		
		uint8_t data[8];

		uint16_t calc_crc16(const uint8_t *buf, size_t len);
		uint16_t combine_bytes(uint8_t msb, uint8_t lsb);
	
		void validate_sensor_data();

	public:
	
		temp_humid_sensor(int gpio_pin);

		void reset();
		void read_data();
		bool valid();

		float get_temperature_in_c();
		float get_temperature_in_f();
		float get_humidity();
		int   get_error();
		
	protected:

};

temp_humid_sensor::temp_humid_sensor(int gpio_pin) {
	
	
}

void temp_humid_sensor::reset() {
	
	this->sensor_status = 0;
	
	
	
	this->fd = open(I2C_DEVICE, O_RDWR);
	
	if (this->fd < 0) {
		this->sensor_status = 1;
		return;
	}
	
	if (ioctl(this->fd, I2C_SLAVE, AM2321_ADDR) < 0) {
		this->sensor_status = 2;
		return;
	}
   
	/* wake AM2320 up, goes to sleep to not warm up and
	* affect the humidity sensor 
	*/
	write(this->fd, NULL, 0);
	usleep(1000); /* at least 0.8ms, at most 3ms */
	
	/* write at addr 0x03, start reg = 0x00, num regs = 0x04 */

	this->data[0] = 0x03; 
	this->data[1] = 0x00; 
	this->data[2] = 0x04;
	
	if (write(this->fd, this->data, 3) < 0) {
		this->sensor_status = 3;
		return;
	}

}

uint16_t temp_humid_sensor::calc_crc16(const uint8_t *buf, size_t len) {

  uint16_t crc = 0xFFFF;
  
  while(len--) {
    crc ^= (uint16_t) *buf++;
    for (unsigned i = 0; i < 8; i++) {
      if (crc & 0x0001) {
		crc >>= 1;
		crc ^= 0xA001;
      } else {
		crc >>= 1;      
      }
    }
  }
  
  return crc;
}

uint16_t temp_humid_sensor::combine_bytes(uint8_t msb, uint8_t lsb)
{
  return ((uint16_t)msb << 8) | (uint16_t)lsb;
}


void temp_humid_sensor::validate_sensor_data(){
	
	this->data_valid = false;

	/* Check data[0] and data[1] */
	if (this->data[0] != 0x03 || this->data[1] != 0x04) {
		this->sensor_status = 9;
		return;
	}

	/* Check CRC */
	uint16_t crcdata = this->calc_crc16(this->data, 6);
	uint16_t crcread = this->combine_bytes(this->data[7], this->data[6]);
	
	if (crcdata != crcread) {
		this->sensor_status = 10;
		return;	
	}

	this->data_valid = true;
}

bool temp_humid_sensor::valid(){
	return this->data_valid;
}

void temp_humid_sensor::read_data(){
	
	this->data_valid = false;
	
	this->reset();

	if (this->sensor_status != 0 ) return;
  
	/* wait for AM2320 */
	usleep(1600); /* Wait atleast 1.5ms */

	/*
	* Read out 8 bytes of data
	* Byte 0: Should be Modbus function code 0x03
	* Byte 1: Should be number of registers to read (0x04)
	* Byte 2: Humidity msb
	* Byte 3: Humidity lsb
	* Byte 4: Temperature msb
	* Byte 5: Temperature lsb
	* Byte 6: CRC lsb byte
	* Byte 7: CRC msb byte
	*/

	if (read(this->fd, this->data, 8) < 0) {
		this->sensor_status = 4;
		close(this->fd);
		return;
	}
	
	close(fd);

  //printf("[0x%02x 0x%02x  0x%02x 0x%02x  0x%02x 0x%02x  0x%02x 0x%02x]\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] );



  uint16_t temp16 = this->combine_bytes(data[4], data[5]); 
  uint16_t humi16 = this->combine_bytes(data[2], data[3]);   

    // Check MSB vor 1 (negative temperature) 
	if (temp16 & 0x8000) temp16 = -(temp16 & 0x7FFF);
 
	this->validate_sensor_data();

	if(this->data_valid) {
		this->humidity    = (float)humi16 / 10.0;
		this->temperature = (float)temp16 / 10.0;
	}

}

int temp_humid_sensor::get_error(){
	return this->sensor_status;
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


	temp_humid_sensor sensor( 0 );

	for ( int read_counter = 0; read_counter < 10; read_counter++ )
	{

		sensor.read_data();
		if(sensor.valid())
            printf( "Humidity = %f%% Temperature = %f C \n", sensor.get_humidity(), sensor.get_temperature_in_c());
        else
			printf( "Error %d \n", sensor.get_error());

		usleep(1000000); 
	}

	return(0);
}
