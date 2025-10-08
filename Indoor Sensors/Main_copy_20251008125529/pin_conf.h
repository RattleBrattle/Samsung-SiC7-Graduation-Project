#ifndef PIN_CONF_H
#define PIN_CONF_H

// Sensor pins (Readings)
#define DHT_PIN 4
#define PIR_PIN 5
#define LDR_PIN 34
#define MQ2_PIN 35
#define MQ135_PIN 32

// Motor pins for L293D (Fan & AC)
#define MOTOR1_IN1 13
#define MOTOR1_IN2 12
#define MOTOR1_EN 14
#define MOTOR2_IN1 27
#define MOTOR2_IN2 26
#define MOTOR2_EN 25

// LED pins (Room lights)
#define LED1_GPIO 15
#define LED2_GPIO 2
#define LED3_GPIO 18

// DHT sensor type
#define DHT_TYPE DHT11

// Auto light settings (LDR)
#define LIGHT_INTENSITY_THRESHOLD 30

#endif