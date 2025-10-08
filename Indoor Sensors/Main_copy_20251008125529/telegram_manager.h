#ifndef TELEGRAM_MANAGER_H
#define TELEGRAM_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"

class TelegramManager {
  public:
    TelegramManager();
    void begin();
    bool sendMessage(String message);
    bool sendAlarm(String alarmType, String details);
    
  private:
    WiFiClientSecure client;
    String botToken;
    String chatID;
    
    bool connectToTelegram();
};

#endif