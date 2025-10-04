#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
const char* ssid = "Fast Food";
const char* password = "Amanybrma2023+";

// ThingsBoard credentials
const char* mqtt_server = "mqtt.thingsboard.cloud";
const char* mqtt_token = "9ojgb2333x7gt5i9rgmo";
const char* mqtt_client_id = "ESP32_SmartHome";

// Sensor Update readings interval
const long SENSOR_READ_INTERVAL = 5000;

#endif
