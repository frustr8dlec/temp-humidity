// Developer note stack size needs to be increased to allow the allocation of memory for this double = 8 bytes
// 8 * 24 * 60 * 60 * 2 arrays of doubles = 1382400 bytes base allocation is 1MB 
// In VS2017 go to project > properties > System > and set Stack Reserve Size and Stack Commit Size to 2000000
// This prevents a stack overflow

#define maximum_readings 24 * 60 * 60
#define invalid_reading  -1000.0

#include <chrono>  // System time

#if windows == 1
#include "http.h"
#else
#include "am2320.h"
#endif

// 2.1+ Makes use of a vector for the storage of data allowing the duration of the samples to be increased beyond fixed 24 hour period.
//#include <vector> 

#include <stdexcept>

using namespace std;
using namespace chrono;

/* 2.2+ Creates a type using a struct to hold all data items.
typedef  struct reading {
	long		sampleSeconds;				 // Sample time in seconds from start of program
	system_clock::time_point sampleTime; // Real sample time
	double	humidity;					 // Humidity in %
	double	temperature;				 // Temp in degrees C
	bool	validReading;				 // True = a valid reading if in array structure
};
*/

/* 2.1+ Extends struct to a class
class reading {

private:
	long		sampleSeconds;				 // Sample time in seconds from start of program
	system_clock::time_point sampleTime; // Real sample time
	double	humidity;					 // Humidity in %
	double	temperature;				 // Temp in degrees C
	bool	validReading;				 // True = a valid reading if in array structure
protected:

public:
	// Constructor and methods to support above

};
*/

class Climate {

private:

	// Instance of the sensor class

#if windows == 1
	Http sensorDevice;
#else
	Am2320 sensorDevice;
#endif

	// Time related variables
	system_clock::time_point startTime;
	system_clock::time_point lastSampleTime;
	system_clock::time_point currentTime;

	long	totalSamples;

	// Sensor data records 3rd
	double humidity[maximum_readings];
	double temperature[maximum_readings];

	// Sensor data records 2.2
	//reading readings[maximum_readings];

	// Sensor data records 2.1+
	//vector<reading> readings;
	//long maximum_readings;

	// Utility methods
	void resetData();

	// Range checking methods
	bool within24HourSampleLimit();
	bool sampleOneSecondDelayExpired();

	long	 calcTimeDifferenceToArrayIndex(system_clock::time_point, system_clock::time_point);

	// Statistical methods
	// Parameters are:

	// data			reference to the array of doubles to work with
	// startElement	first element to work with 0 to maximum_readings
	// endElement	last element to work with 0 to maximum_readings

	double averageArraySection(double(&data)[maximum_readings], long startElement, long endElement);
	double maximumArraySection(double(&data)[maximum_readings], long startElement, long endElement);
	double minimumArraySection(double(&data)[maximum_readings], long startElement, long endElement);

protected:

public:

	// Constructors
	Climate();

	// Utility
	void	clearSamples();
	long	sampleCount(long);
	long	sampleTotal();

	// () operator overload
	double operator() (const string measurement, long element) const {

		if (measurement == "humidity")	 return this->humidity[element];
		if (measurement == "temperature") return this->temperature[element];

		return invalid_reading;
	}


	// Sensor related
	long		readSensor();

	// Humidity methods
	double	getHumidity(long);
	double	averageHumidity(long);
	double	maximumHumidity(long);
	double	minimumHumidity(long);

	// Temperature methods 
	double	getTemperature(long);
	double	averageTemperature(long);
	double	maximumTemperature(long);
	double	minimumTemperature(long);
};

// Constructor
Climate::Climate() {
	this->resetData();
}

// Intialise the data structure
void	Climate::resetData() {

	this->clearSamples();

	this->startTime = std::chrono::system_clock::now();
	this->lastSampleTime = std::chrono::system_clock::now();

	this->totalSamples = 0;

}

// Test to see if the 24 hour sample period has expired
bool	Climate::within24HourSampleLimit() {
	std::chrono::duration<double> secondsElapsedFromFirstReading = this->currentTime - this->startTime;
	return secondsElapsedFromFirstReading.count() < maximum_readings;
}


// Test to see if the one second delay has occurred before sampling again
bool	Climate::sampleOneSecondDelayExpired() {
	std::chrono::duration<double> secondsElapsedFromLastReading = this->currentTime - this->lastSampleTime;
	return secondsElapsedFromLastReading.count() >= 1;
}

// Find the average of all valid entries in the array section
double Climate::averageArraySection(double(&data)[maximum_readings], long startElement, long endElement) {

	long totalReadingsInRange = 0;
	double total = 0;

	for (long element = startElement; element < endElement; element++) {
		if (data[element] != invalid_reading) {
			total += data[element];
			totalReadingsInRange++;
		}
	}

	return total / (double)totalReadingsInRange;
}

// Find the maximum of all valid entries in the array section
double Climate::maximumArraySection(double(&data)[maximum_readings], long startElement, long endElement) {

	double maximumReadingInRange = 0;

	for (long element = startElement; element < endElement; element++) {
		if (data[element] != invalid_reading && data[element] > maximumReadingInRange) {
			maximumReadingInRange = data[element];
		}
	}

	return maximumReadingInRange;
}

// Find the minimum of all valid entries in the array section
double Climate::minimumArraySection(double(&data)[maximum_readings], long startElement, long endElement) {

	double minimumReadingInRange = 1000.0;

	for (long element = startElement; element < endElement; element++) {
		if (data[element] != invalid_reading && data[element] < minimumReadingInRange) {
			minimumReadingInRange = data[element];
		}
	}

	return minimumReadingInRange;
}


// Set all the samples to be invalid_readings
void	Climate::clearSamples() {
	for (long index = 0; index < maximum_readings; index++) {
		this->humidity[index] = invalid_reading;
		this->temperature[index] = invalid_reading;
	}
}

// Read the sensor throwing errors where limits are not met or sensor not available, returns the second that the sample was taken
long	Climate::readSensor() {

	this->currentTime = std::chrono::system_clock::now();

	if (!within24HourSampleLimit())		throw std::out_of_range("24 Hour limit reached.");
	if (!sampleOneSecondDelayExpired()) throw std::underflow_error("Delay between sensor read is below 1 second.");

	std::chrono::duration<double> sampleTime = this->currentTime - this->startTime;

	int currentSecond = (long)sampleTime.count();

	sensorDevice.read_data();

	if (sensorDevice.valid()) {
		// Store readings
		this->humidity[currentSecond] = sensorDevice.get_humidity();
		this->temperature[currentSecond] = sensorDevice.get_temperature_in_c();
		// Increase the sample counter
		this->totalSamples++;
	}
	else
		throw std::runtime_error("Invalid Sensor Read ");

	return currentSecond;

}

// Work out the index from the difference between two system clock times
long Climate::calcTimeDifferenceToArrayIndex(system_clock::time_point timeStart, system_clock::time_point timeEnd) {

	std::chrono::duration<double> indexDuration = timeEnd - timeStart;

	long index = (long)indexDuration.count();

	if (index < 0) index = 0;

	return index;
}

// Loop through the array counting valid samples over the number of seconds.
long		Climate::sampleCount(long duration_in_seconds) {

	long numberOfSamples = 0;

	// Set the current time
	this->currentTime = std::chrono::system_clock::now();

	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime); // Last array element to read
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	for (long index = startIndex; ((index <= endIndex) && (index < maximum_readings)); index++) {
		if (this->humidity[index] != invalid_reading)
			numberOfSamples++;
	}

	return numberOfSamples;
}


// Return the total number of samples taken
long		Climate::sampleTotal() {
	return this->totalSamples;
}

/*
   -------------------------------------------------------------------------------------------
   Humidity methods
   -------------------------------------------------------------------------------------------
*/

double	Climate::getHumidity(long time_in_seconds) {
	if (time_in_seconds < 0 || time_in_seconds >= maximum_readings)
		throw std::out_of_range(" Not a valid time");

	if (this->temperature[time_in_seconds] == invalid_reading)
		throw std::invalid_argument(" No valid reading for this time");

	return this->humidity[time_in_seconds];
}

// calculate the average of measurements from the current time
double	Climate::averageHumidity(long duration_in_seconds) {

	this->currentTime = std::chrono::system_clock::now();
	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime);
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	return averageArraySection(this->humidity, startIndex, endIndex);
}

// Find the maximum humidity from the current time
double	Climate::maximumHumidity(long duration_in_seconds) {
	this->currentTime = std::chrono::system_clock::now();
	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime);
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	return maximumArraySection(this->humidity, startIndex, endIndex);
}

double	Climate::minimumHumidity(long duration_in_seconds) {
	this->currentTime = std::chrono::system_clock::now();
	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime);
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	return minimumArraySection(this->humidity, startIndex, endIndex);
}

/*
   -------------------------------------------------------------------------------------------
   Temperature methods
   -------------------------------------------------------------------------------------------
*/

double	Climate::getTemperature(long time_in_seconds) {
	if (time_in_seconds < 0 || time_in_seconds >= maximum_readings)
		throw std::out_of_range(" Not a valid time");

	if (this->temperature[time_in_seconds] == invalid_reading)
		throw std::invalid_argument(" No valid reading for this time");

	return this->temperature[time_in_seconds];
}

// calculate the average of measurements from the current time
double	Climate::averageTemperature(long duration_in_seconds) {

	this->currentTime = std::chrono::system_clock::now();
	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime);
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	return averageArraySection(this->temperature, startIndex, endIndex);
}

// Find the maximum temperature from the current time
double	Climate::maximumTemperature(long duration_in_seconds) {
	this->currentTime = std::chrono::system_clock::now();
	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime);
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	return maximumArraySection(this->temperature, startIndex, endIndex);
}

double	Climate::minimumTemperature(long duration_in_seconds) {
	this->currentTime = std::chrono::system_clock::now();
	long endIndex = calcTimeDifferenceToArrayIndex(this->startTime, this->currentTime);
	long startIndex = endIndex - duration_in_seconds;

	if (startIndex < 0)
		startIndex = 0;

	return minimumArraySection(this->temperature, startIndex, endIndex);
}
