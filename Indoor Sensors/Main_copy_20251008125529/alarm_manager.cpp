#include "alarm_manager.h"
#include <Arduino.h>

constexpr float AlarmManager::TEMP_HIGH_THRESHOLD;
constexpr float AlarmManager::TEMP_LOW_THRESHOLD;
constexpr float AlarmManager::HUMIDITY_HIGH_THRESHOLD;
constexpr float AlarmManager::HUMIDITY_LOW_THRESHOLD;
constexpr int AlarmManager::SMOKE_THRESHOLD;
constexpr int AlarmManager::AIR_QUALITY_THRESHOLD;

AlarmManager::AlarmManager() {
  clearAlarms();
  lastMotionAlert = 0;
}

void AlarmManager::checkAlarms(float temperature, float humidity, int smoke, int airQuality, bool motion) {
  checkTemperatureAlarms(temperature);
  checkHumidityAlarms(humidity);
  checkSmokeAlarm(smoke);
  checkAirQualityAlarm(airQuality);
  checkMotionAlarm(motion);
}

void AlarmManager::checkTemperatureAlarms(float temperature) {
  tempHighAlarm = (temperature > TEMP_HIGH_THRESHOLD);
  tempLowAlarm = (temperature < TEMP_LOW_THRESHOLD);
}

void AlarmManager::checkHumidityAlarms(float humidity) {
  humidityHighAlarm = (humidity > HUMIDITY_HIGH_THRESHOLD);
  humidityLowAlarm = (humidity < HUMIDITY_LOW_THRESHOLD);
}

void AlarmManager::checkSmokeAlarm(int smoke) {
  smokeAlarm = (smoke > SMOKE_THRESHOLD);
}

void AlarmManager::checkAirQualityAlarm(int airQuality) {
  airQualityAlarm = (airQuality > AIR_QUALITY_THRESHOLD);
}

void AlarmManager::checkMotionAlarm(bool motion) {
  unsigned long currentTime = millis();
  if (motion && (currentTime - lastMotionAlert > MOTION_ALERT_COOLDOWN)) {
    motionAlarm = true;
    lastMotionAlert = currentTime;
  } else {
    motionAlarm = false;
  }
}

String AlarmManager::getActiveAlarms() {
  String alarms = "";
  
  if (tempHighAlarm) alarms += "HIGH_TEMP,";
  if (tempLowAlarm) alarms += "LOW_TEMP,";
  if (humidityHighAlarm) alarms += "HIGH_HUMIDITY,";
  if (humidityLowAlarm) alarms += "LOW_HUMIDITY,";
  if (smokeAlarm) alarms += "SMOKE_DETECTED,";
  if (airQualityAlarm) alarms += "POOR_AIR_QUALITY,";
  if (motionAlarm) alarms += "MOTION_DETECTED,";
  
  // Remove trailing comma
  if (alarms.length() > 0) {
    alarms.remove(alarms.length() - 1);
  }
  
  return alarms;
}

void AlarmManager::clearAlarms() {
  tempHighAlarm = false;
  tempLowAlarm = false;
  humidityHighAlarm = false;
  humidityLowAlarm = false;
  smokeAlarm = false;
  airQualityAlarm = false;
  motionAlarm = false;
}

bool AlarmManager::hasActiveAlarms() {
  return tempHighAlarm || tempLowAlarm || humidityHighAlarm || 
         humidityLowAlarm || smokeAlarm || airQualityAlarm || motionAlarm;
}