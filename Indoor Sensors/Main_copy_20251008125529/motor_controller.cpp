#include "motor_controller.h"
#include <Arduino.h>

// Constructor
MotorController::MotorController() {
}

void MotorController::begin() {
  initializePins();
  stopMotor1();
  stopMotor2();
}

void MotorController::initializePins() {
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR1_EN, OUTPUT);
  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);
  pinMode(MOTOR2_EN, OUTPUT);
}

void MotorController::startMotor1() {
  digitalWrite(MOTOR1_IN1, HIGH);
  digitalWrite(MOTOR1_IN2, LOW);
  analogWrite(MOTOR1_EN, DEFAULT_SPEED);
  Serial.println("Motor 1 (Fan) Started");
}

void MotorController::stopMotor1() {
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, LOW);
  analogWrite(MOTOR1_EN, 0);
  Serial.println("Motor 1 (Fan) Stopped");
}

void MotorController::startMotor2() {
  digitalWrite(MOTOR2_IN1, HIGH);
  digitalWrite(MOTOR2_IN2, LOW);
  analogWrite(MOTOR2_EN, DEFAULT_SPEED);
  Serial.println("Motor 2 (AC) Started");
}

void MotorController::stopMotor2() {
  digitalWrite(MOTOR2_IN1, LOW);
  digitalWrite(MOTOR2_IN2, LOW);
  analogWrite(MOTOR2_EN, 0);
  Serial.println("Motor 2 (AC) Stopped");
}

void MotorController::setMotor1Speed(int speed) {
  speed = constrain(speed, 0, 255);
  analogWrite(MOTOR1_EN, speed);
  Serial.print("Motor 1 speed set to: ");
  Serial.println(speed);
}

void MotorController::setMotor2Speed(int speed) {
  speed = constrain(speed, 0, 255);
  analogWrite(MOTOR2_EN, speed);
  Serial.print("Motor 2 speed set to: ");
  Serial.println(speed);
}