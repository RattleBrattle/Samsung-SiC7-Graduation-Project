#include "wifi_mqtt.h"
#include "sensor_functions.h"
#include "motor_controller.h"
#include "led_controller.h"
#include "alarm_manager.h"
#include "telegram_manager.h"
#include "config.h"
#include <ArduinoJson.h>

// Static callback wrapper
WiFiMQTTManager* wifiMqttInstance = nullptr;

// Constructor
WiFiMQTTManager::WiFiMQTTManager(SensorManager& sensorMgr, MotorController& motorCtrl, 
                               LEDController& ledCtrl, AlarmManager& alarmMgr, TelegramManager& telegramMgr) 
  : sensorManager(sensorMgr), motorController(motorCtrl), ledController(ledCtrl), 
    alarmManager(alarmMgr), telegramManager(telegramMgr), client(espClient) {
  previousMillis = 0;
  wifiMqttInstance = this;
}

void WiFiMQTTManager::begin() {
  setupWiFi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void WiFiMQTTManager::setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiMQTTManager::reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_TOKEN, NULL)) {
      Serial.println("connected");
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void WiFiMQTTManager::callback(char* topic, byte* payload, unsigned int length) {
  if (wifiMqttInstance != nullptr) {
    wifiMqttInstance->handleMessage(topic, payload, length);
  }
}

void WiFiMQTTManager::handleMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);
  
  // Handle RPC methods
  if (doc.containsKey("method")) {
    String method = doc["method"];
    
    if (method == "setState" && doc.containsKey("params")) {
      JsonObject params = doc["params"];
      
      // Fan control
      if (params.containsKey("fan")) {
        String fanState = params["fan"];
        if (fanState == "true" || fanState == "ON") {
          motorController.startMotor1();
          Serial.println("Fan turned ON via RPC");
        } else if (fanState == "false" || fanState == "OFF") {
          motorController.stopMotor1();
          Serial.println("Fan turned OFF via RPC");
        }
      }
      
      // AC control
      if (params.containsKey("ac")) {
        String acState = params["ac"];
        if (acState == "true" || acState == "ON") {
          motorController.startMotor2();
          Serial.println("AC turned ON via RPC");
        } else if (acState == "false" || acState == "OFF") {
          motorController.stopMotor2();
          Serial.println("AC turned OFF via RPC");
        }
      }
      
      // LED control
      if (params.containsKey("led1")) {
        String ledState = params["led1"];
        bool state = (ledState == "true" || ledState == "ON");
        ledController.setLED(1, state);
        Serial.print("LED1 set to: ");
        Serial.println(state ? "ON" : "OFF");
      }
      
      if (params.containsKey("led2")) {
        String ledState = params["led2"];
        bool state = (ledState == "true" || ledState == "ON");
        ledController.setLED(2, state);
        Serial.print("LED2 set to: ");
        Serial.println(state ? "ON" : "OFF");
      }
      
      if (params.containsKey("led3")) {
        String ledState = params["led3"];
        bool state = (ledState == "true" || ledState == "ON");
        ledController.setLED(3, state);
        Serial.print("LED3 set to: ");
        Serial.println(state ? "ON" : "OFF");
      }
      
      // LED manual/auto mode
      if (params.containsKey("ledMode")) {
        String mode = params["ledMode"];
        bool isManual = (mode == "manual");
        ledController.setManualMode(isManual);
        Serial.print("LED mode set to: ");
        Serial.println(isManual ? "MANUAL" : "AUTO");
      }
    }
    
    // Handle getValue method for LED Mode toggle switch
    else if (method == "getValue") {
      Serial.println("getValue RPC method received - sending current states");
      
      // Extract request ID from topic to send response
      String topicStr = String(topic);
      int lastSlash = topicStr.lastIndexOf('/');
      String requestId = topicStr.substring(lastSlash + 1);
      
      // Create response with current states
      DynamicJsonDocument responseDoc(512);
      responseDoc["led_mode"] = ledController.isManualMode() ? "manual" : "auto";
      
      String response;
      serializeJson(responseDoc, response);
      
      String responseTopic = "v1/devices/me/rpc/response/" + requestId;
      client.publish(responseTopic.c_str(), response.c_str());
      
      Serial.print("Sent getValue response: ");
      Serial.println(response);
    }
    
    // Handle getState request for power buttons (widgets)
    else if (method == "getState") {
      Serial.println("getState RPC method received - sending device states");
      
      // Extract request ID from topic to send response
      String topicStr = String(topic);
      int lastSlash = topicStr.lastIndexOf('/');
      String requestId = topicStr.substring(lastSlash + 1);
      
      // Response with current device states
      DynamicJsonDocument responseDoc(512);
      responseDoc["fan"] = false;    // Default to off
      responseDoc["ac"] = false;     // Default to off
      responseDoc["led1"] = ledController.getLEDState(1);
      responseDoc["led2"] = ledController.getLEDState(2);
      responseDoc["led3"] = ledController.getLEDState(3);
      responseDoc["ledMode"] = ledController.isManualMode() ? "manual" : "auto";
      
      String response;
      serializeJson(responseDoc, response);
      
      String responseTopic = "v1/devices/me/rpc/response/" + requestId;
      client.publish(responseTopic.c_str(), response.c_str());
      
      Serial.print("Sent getState response: ");
      Serial.println(response);
    }
  }
}

void WiFiMQTTManager::checkAndSendAlarms(float temperature, float humidity, int smoke, int airQuality, bool motion) {
  // Check for alarms
  alarmManager.checkAlarms(temperature, humidity, smoke, airQuality, motion);
  
  // Send to Telegram if any alarms
  if (alarmManager.hasActiveAlarms()) {
    String activeAlarms = alarmManager.getActiveAlarms();
    
    // Send to Telegram
    telegramManager.sendAlarm("Multiple Alarms", activeAlarms);
    
    Serial.println("Active Alarms: " + activeAlarms);
  }
}

void WiFiMQTTManager::publishSensorData() {
  float temperature, humidity;
  bool motion;
  int light, smoke, airQuality;
  
  sensorManager.getAllReadings(temperature, humidity, motion, light, smoke, airQuality);
  
  // Check and send alarms
  checkAndSendAlarms(temperature, humidity, smoke, airQuality, motion);
  
  // Auto control LEDs based on light intensity (if not in manual mode)
  ledController.autoControlLEDs(light);
  
  DynamicJsonDocument doc(1024);
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["motion"] = motion;
  doc["light_intensity"] = light;
  doc["smoke_level"] = smoke;
  doc["air_quality"] = airQuality;
  doc["led1"] = ledController.getLEDState(1);
  doc["led2"] = ledController.getLEDState(2);
  doc["led3"] = ledController.getLEDState(3);
  doc["led_mode"] = ledController.isManualMode() ? "manual" : "auto";
  doc["active_alarms"] = alarmManager.getActiveAlarms();
  doc["has_alarms"] = alarmManager.hasActiveAlarms();
  
  String telemetry;
  serializeJson(doc, telemetry);
  
  client.publish("v1/devices/me/telemetry", telemetry.c_str());
  
  Serial.println("Published sensor data to ThingsBoard:");
  Serial.println(telemetry);
}

void WiFiMQTTManager::loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= SENSOR_READ_INTERVAL) {
    previousMillis = currentMillis;
    publishSensorData();
  }
}

bool WiFiMQTTManager::isConnected() {
  return client.connected();
}