class Sensor {

	private:
	
	
	protected:

		bool data_valid;
		
		float temperature;
		float humidity;
			
		int sensor_status;
		
		virtual void validate_sensor_data() = 0;
		
	public:

		static float convert_c_to_f(float c) {return c * 9.0 / 5.0 + 32;}

		virtual void reset() = 0;

		virtual void read_data() = 0;

		virtual bool valid() {return this->data_valid;}

		int get_error() {return this->sensor_status;}

		float get_humidity() {return this->humidity;}
		
		float get_temperature_in_c() {return this->temperature;}
		
		float get_temperature_in_f() {
			return this->convert_c_to_f(this->temperature);
		}
};
