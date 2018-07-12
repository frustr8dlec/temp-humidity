/*  
 * Reads an AM2320 or AM2321 sensor using I2C bus on the raspberry PI
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

#include <sys/ioctl.h>
#include <fcntl.h> 
#include <linux/i2c-dev.h>

#include "am2320.h"

Am2320::Am2320(){
	this->reset();
}

void Am2320::reset() {
	
	// Clear existing data
	this->data[0] = this->data[1] = this->data[2] 
		= this->data[3] = this->data[4] = this->data[5] 
		= this->data[6] = this->data[7] = 0;
		
	this->temperature = 0.0;
	this->humidity = 0.0;
	this->data_valid = false;
	
	this->sensor_status = 0;
	
	this->i2c_bus_data = open(I2C_DEVICE, O_RDWR);
	
	if (this->i2c_bus_data < 0) {
		this->sensor_status = 1;
		return;
	}
	
	if (ioctl(this->i2c_bus_data, I2C_SLAVE, AM2321_ADDR) < 0) {
		this->sensor_status = 2;
		return;
	}
   
	/* wake AM2320 up, goes to sleep to not warm up and
	* affect the humidity sensor 
	*/
	write(this->i2c_bus_data, NULL, 0);
	usleep(1000); /* at least 0.8ms, at most 3ms */
	
	/* write at addr 0x03, start reg = 0x00, num regs = 0x04 */

	this->data[0] = 0x03; 
	this->data[1] = 0x00; 
	this->data[2] = 0x04;
	
	if (write(this->i2c_bus_data, this->data, 3) < 0) {
		this->sensor_status = 3;
		return;
	}

}

uint16_t Am2320::calc_crc16(const uint8_t *buf, size_t len) {

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

uint16_t Am2320::combine_bytes(uint8_t msb, uint8_t lsb)
{
  return ((uint16_t)msb << 8) | (uint16_t)lsb;
}


void Am2320::validate_sensor_data(){
	
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

void Am2320::read_data(){
	
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

	if (read(this->i2c_bus_data, this->data, 8) < 0) {
		this->sensor_status = 4;
		close(this->i2c_bus_data);
		return;
	}
	
	close(i2c_bus_data);

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


