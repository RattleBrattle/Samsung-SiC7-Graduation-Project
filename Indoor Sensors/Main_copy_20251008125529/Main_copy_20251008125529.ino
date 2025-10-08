#include "config.h"
#include "sensor_functions.h"
#include "motor_controller.h"
#include "led_controller.h"
#include "alarm_manager.h"
#include "telegram_manager.h"
#include "wifi_mqtt.h"

// Create instances
SensorManager sensorManager;
MotorController motorController;
LEDController ledController;
AlarmManager alarmManager;
TelegramManager telegramManager;
WiFiMQTTManager wifiMQTT(sensorManager, motorController, ledController, alarmManager, telegramManager);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing Smart Home System...");
  
  // Initialize components
  sensorManager.begin();
  motorController.begin();
  ledController.begin();
  telegramManager.begin();
  wifiMQTT.begin();
  
  // Send startup message to Telegram
  telegramManager.sendMessage("Smart Home System Started\nAll sensors initialized and ready!");
  
  Serial.println("System initialization complete!");
  Serial.println("Connecting to ThingsBoard...");
}

void loop() {
  wifiMQTT.loop();
}