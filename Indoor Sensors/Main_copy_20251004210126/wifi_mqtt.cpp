#include "wifi_mqtt.h"
#include "sensor_functions.h"
#include "motor_controller.h"
#include "led_controller.h"
#include "config.h"
#include <ArduinoJson.h>

// Static callback wrapper
WiFiMQTTManager* wifiMqttInstance = nullptr;

WiFiMQTTManager::WiFiMQTTManager(SensorManager& sensorMgr, MotorController& motorCtrl, LEDController& ledCtrl) 
  : sensorManager(sensorMgr), motorController(motorCtrl), ledController(ledCtrl), client(espClient) {
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
  if (doc.containsKey("method") && doc.containsKey("params")) {
    String method = doc["method"];
    JsonObject params = doc["params"];
    
    if (method == "setState") {
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
    
    // Handle getState request
    else if (method == "getState") {
      Serial.println("getState RPC method received");
      // You can implement response here if needed
    }
  }
}

void WiFiMQTTManager::publishSensorData() {
  float temperature, humidity;
  int light, smoke, airQuality;
  
  sensorManager.getAllReadings(temperature, humidity, light, smoke, airQuality);
  
  // Auto control LEDs based on light intensity (if not in manual mode)
  ledController.autoControlLEDs(light);
  
  DynamicJsonDocument doc(1024);
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["light_intensity"] = light;
  doc["smoke_level"] = smoke;
  doc["air_quality"] = airQuality;
  doc["led1"] = ledController.getLEDState(1);
  doc["led2"] = ledController.getLEDState(2);
  doc["led3"] = ledController.getLEDState(3);
  doc["led_mode"] = ledController.isManualMode() ? "manual" : "auto";
  
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