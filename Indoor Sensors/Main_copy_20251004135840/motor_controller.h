#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "pins.h"

#define DEFAULT_SPEED 200

/* Class for controlling the two DC motors (Fan & AC)
*  Public Functions:
*     1. begin(): Assigns Pins (IN and EN) for each motor.
*     2. startMotor1(): gives power to Motor 1 to start. 
*     3. stopMotor1(): stops Motor 1.
*     2. startMotor2(): gives power to Motor 2 to start. 
*     3. stopMotor2(): stops Motor 2.
*     6. setMotor1Speed(): Controls the Motor 1 speed by PWM.
*     7. setMotor2Speed(): Controls the Motor 2 speed by PWM.
*/
class MotorController {
  public:
    MotorController();
    void begin();
    void startMotor1();
    void stopMotor1();
    void startMotor2();
    void stopMotor2();
    void setMotor1Speed(int speed);
    void setMotor2Speed(int speed);
    
  private:
    void initializePins();
};

#endif