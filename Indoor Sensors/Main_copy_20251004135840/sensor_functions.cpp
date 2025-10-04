#include "sensor_functions.h"
#include <Arduino.h>

SensorManager::SensorManager() : dht(DHT_PIN, DHT_TYPE) {
  lastMotionState = false;
}

void SensorManager::begin() {
  dht.begin();
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);
}

float SensorManager::readTemperature() {
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("Failed to read temperature from DHT11!");
    return -1;
  }
  return temp;
}

float SensorManager::readHumidity() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed to read humidity from DHT11!");
    return -1;
  }
  return humidity;
}

bool SensorManager::readMotion() {
  return digitalRead(PIR_PIN);
}

int SensorManager::readLightIntensity() {
  int ldrValue = analogRead(LDR_PIN);
  int percentage = map(ldrValue, 0, 4095, 0, 100);
  return percentage;
}

int SensorManager::readSmokeLevel() {
  int mq2Value = analogRead(MQ2_PIN);
  int percentage = map(mq2Value, 0, 4095, 0, 100);
  return percentage;
}

int SensorManager::readAirQuality() {
  int mq135Value = analogRead(MQ135_PIN);
  int percentage = map(mq135Value, 0, 4095, 0, 100);
  return percentage;
}

void SensorManager::getAllReadings(float &temp, float &humidity, bool &motion, int &light, int &smoke, int &airQuality) {
  temp = readTemperature();
  humidity = readHumidity();
  motion = readMotion();
  light = readLightIntensity();
  smoke = readSmokeLevel();
  airQuality = readAirQuality();
  
  // Check motion state for PIR
  if (motion != lastMotionState) {
    if (motion) {
      Serial.println("Motion detected!");
    } else {
      Serial.println("Motion ended!");
    }
    lastMotionState = motion;
  }
}