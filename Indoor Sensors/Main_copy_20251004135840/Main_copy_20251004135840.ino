#include "config.h"
#include "sensor_functions.h"
#include "motor_controller.h"
#include "wifi_mqtt.h"

// Create class instances
SensorManager sensorManager;
MotorController motorController;
WiFiMQTTManager wifiMQTT(sensorManager, motorController);

void setup() {
  Serial.begin(115200);
  Serial.println("Start Smart Home Sensor Readings System...");
  
  // Initialize components
  sensorManager.begin();
  motorController.begin();
  wifiMQTT.begin();
  
  Serial.println("System initialization complete!");
}

void loop() {
  wifiMQTT.loop();
}