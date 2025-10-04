#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Forward declarations
class SensorManager;
class MotorController;

class WiFiMQTTManager {
  public:
    WiFiMQTTManager(SensorManager& sensorMgr, MotorController& motorCtrl);
    void begin();
    void loop();
    void publishSensorData();
    bool isConnected();
    
  private:
    WiFiClient espClient;
    PubSubClient client;
    SensorManager& sensorManager;
    MotorController& motorController;
    unsigned long previousMillis;
    
    void setupWiFi();
    void reconnect();
    static void callback(char* topic, byte* payload, unsigned int length);
    void handleMessage(char* topic, byte* payload, unsigned int length);
};

#endif