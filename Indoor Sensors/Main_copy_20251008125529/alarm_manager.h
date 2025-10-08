#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <Arduino.h>

class AlarmManager {
  public:
    AlarmManager();
    void checkAlarms(float temperature, float humidity, int smoke, int airQuality, bool motion);
    String getActiveAlarms();
    void clearAlarms();
    bool hasActiveAlarms();
    
    // Alarm thresholds - define as static constexpr
    static constexpr float TEMP_HIGH_THRESHOLD = 35.0;
    static constexpr float TEMP_LOW_THRESHOLD = 10.0;
    static constexpr float HUMIDITY_HIGH_THRESHOLD = 80.0;
    static constexpr float HUMIDITY_LOW_THRESHOLD = 20.0;
    static constexpr int SMOKE_THRESHOLD = 50;
    static constexpr int AIR_QUALITY_THRESHOLD = 70;
    
  private:
    bool tempHighAlarm;
    bool tempLowAlarm;
    bool humidityHighAlarm;
    bool humidityLowAlarm;
    bool smokeAlarm;
    bool airQualityAlarm;
    bool motionAlarm;
    
    unsigned long lastMotionAlert;
    static const unsigned long MOTION_ALERT_COOLDOWN = 60000; // 1 minute
    
    void checkTemperatureAlarms(float temperature);
    void checkHumidityAlarms(float humidity);
    void checkSmokeAlarm(int smoke);
    void checkAirQualityAlarm(int airQuality);
    void checkMotionAlarm(bool motion);
};

#endif