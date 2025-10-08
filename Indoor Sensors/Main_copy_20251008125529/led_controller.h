#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "pin_conf.h"

class LEDController {
  public:
    LEDController();
    void begin();
    
    // Manual control
    void setLED(int ledNumber, bool state);
    void setAllLEDs(bool state);
    void toggleLED(int ledNumber);
    
    // Auto control based on light intensity
    void autoControlLEDs(int lightIntensity);
    
    // Get current states
    bool getLEDState(int ledNumber);
    
    // Manual override control
    void setManualMode(bool manual);
    bool isManualMode();
    
  private:
    bool ledStates[3];
    bool manualMode; 
    void updateLED(int ledPin, bool state);
};

#endif