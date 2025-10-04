#ifndef SENSOR_FUNCTIONS_H
#define SENSOR_FUNCTIONS_H

#include <DHT.h>
#include "pin_conf.h"

/* Class for reading all each sensor indvidually and controlling the Motors.
*  Public Functions:
*     1. begin(): Assigns Pins for each sensor.
*     2. readTemperature(): Reads the temperature from the DHT11.
*     3. readHumidity(): Reads the humidity from the DHT11.
*     4. readMotion(): If motion was detected or not from the PIR sensor.
*     5. readLightIntensity(): Reads output LDR value.
*     6. readSmokeLevel(): Reads the output from MQ2 sensor.
*     7. readAirQuality(): Reads the air quality value from the MQ135 sensor.
*     8. getAllReadings(): Final function that reads and retrieves values from all sensors connected.
*/
class SensorManager {
  public:
    SensorManager();
    void begin();
    float readTemperature();
    float readHumidity();
    bool readMotion();
    int readLightIntensity();
    int readSmokeLevel();
    int readAirQuality();
    void getAllReadings(float &temp, float &humidity, bool &motion, int &light, int &smoke, int &airQuality);
    
  private:
    DHT dht;
    bool lastMotionState;
};

#endif