#include "wifi_mqtt.h"
#include "sensor_functions.h"
#include "motor_controller.h"
#include "config.h"

// Static callback wrapper
WiFiMQTTManager* wifiMqttInstance = nullptr;

WiFiMQTTManager::WiFiMQTTManager(SensorManager& sensorMgr, MotorController& motorCtrl) 
  : sensorManager(sensorMgr), motorController(motorCtrl), client(espClient) {
  previousMillis = 0;
  wifiMqttInstance = this;
}

void WiFiMQTTManager::begin() {
  setupWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void WiFiMQTTManager::setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
    if (client.connect(mqtt_client_id, mqtt_token, NULL)) {
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
  
  if (doc.containsKey("fan")) {
    String fanState = doc["fan"];
    if (fanState == "ON") {
      motorController.startMotor1();
    } else if (fanState == "OFF") {
      motorController.stopMotor1();
    }
  }
  
  if (doc.containsKey("ac")) {
    String acState = doc["ac"];
    if (acState == "ON") {
      motorController.startMotor2();
    } else if (acState == "OFF") {
      motorController.stopMotor2();
    }
  }
}

void WiFiMQTTManager::publishSensorData() {
  float temperature, humidity;
  bool motion;
  int light, smoke, airQuality;
  
  sensorManager.getAllReadings(temperature, humidity, motion, light, smoke, airQuality);
  
  DynamicJsonDocument doc(1024);
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["motion"] = motion;
  doc["light_intensity"] = light;
  doc["smoke_level"] = smoke;
  doc["air_quality"] = airQuality;
  
  String telemetry;
  serializeJson(doc, telemetry);
  
  client.publish("v1/devices/me/telemetry", telemetry.c_str());
  
  Serial.println("Published sensor data:");
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