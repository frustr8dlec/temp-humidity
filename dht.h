#include "sensor.h"

#define MAXTIMINGS		85
#define DEFAULTDHTPIN	7
#define DATASIZE		5

class  Dht11 : public Sensor {

        private:

		int gpio_pin_number;
		int data[DATASIZE];
		
		void validate_sensor_data();

        public:
        
			Dht11();
			Dht11(int);
			
			void reset();
			void read_data();
			
        protected:

};
