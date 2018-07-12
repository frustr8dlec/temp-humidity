/* g++ -o am2320 am2320.c
 * 
 * Reads an AM2320 or AM2321 using I2C bus on the raspberry PI
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

#include <unistd.h>
#include <stdint.h>

#include "sensor.h"

#define I2C_DEVICE 	"/dev/i2c-1"
#define AM2321_ADDR 0x5C
#define DATASIZE	8

class  Am2320 : public Sensor {

	private:
	
		int i2c_bus_data;
		
		uint8_t data[DATASIZE];

		uint16_t calc_crc16(const uint8_t *buf, size_t len);
		uint16_t combine_bytes(uint8_t msb, uint8_t lsb);
		
	protected:
	
		void validate_sensor_data();

	public:
	
		Am2320();

		void reset();
		void read_data();
		
	protected:

};
