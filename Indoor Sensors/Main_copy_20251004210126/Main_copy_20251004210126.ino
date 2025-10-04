#include "config.h"
#include "sensor_functions.h"
#include "motor_controller.h"
#include "wifi_mqtt.h"

// Create instances
SensorManager sensorManager;
MotorController motorController;
WiFiMQTTManager wifiMQTT(sensorManager, motorController);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing Smart Home System...");
  
  // Initialize components
  sensorManager.begin();
  motorController.begin();
  wifiMQTT.begin();
  
  Serial.println("System initialization complete!");
  Serial.println("Connecting to ThingsBoard...");
}

void loop() {
  wifiMQTT.loop();
}