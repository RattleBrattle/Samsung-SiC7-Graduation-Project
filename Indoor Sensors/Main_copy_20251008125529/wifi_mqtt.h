#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "alarm_manager.h"  
#include "telegram_manager.h"

class SensorManager;
class MotorController;
class LEDController;

class WiFiMQTTManager {
  public:
    WiFiMQTTManager(SensorManager& sensorMgr, MotorController& motorCtrl, 
                   LEDController& ledCtrl, AlarmManager& alarmMgr, TelegramManager& telegramMgr);
    void begin();
    void loop();
    void publishSensorData();
    bool isConnected();
    
  private:
    WiFiClient espClient;
    PubSubClient client;
    SensorManager& sensorManager;
    MotorController& motorController;
    LEDController& ledController;
    AlarmManager& alarmManager;
    TelegramManager& telegramManager;
    unsigned long previousMillis;
    
    void setupWiFi();
    void reconnect();
    static void callback(char* topic, byte* payload, unsigned int length);
    void handleMessage(char* topic, byte* payload, unsigned int length);
    void checkAndSendAlarms(float temperature, float humidity, int smoke, int airQuality, bool motion);
};

#endif