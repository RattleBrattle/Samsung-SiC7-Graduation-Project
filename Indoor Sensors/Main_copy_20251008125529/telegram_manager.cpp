#include "telegram_manager.h"
#include <Arduino.h>

TelegramManager::TelegramManager() {
  botToken = TELEGRAM_BOT_TOKEN;
  chatID = TELEGRAM_CHAT_ID;
}

void TelegramManager::begin() {
  client.setInsecure();
}

bool TelegramManager::connectToTelegram() {
  return client.connect("api.telegram.org", 443);
}

bool TelegramManager::sendMessage(String message) {
  if (!connectToTelegram()) {
    Serial.println("Failed to connect to Telegram");
    return false;
  }
  
  String url = "/bot" + botToken + "/sendMessage";
  String postData = "chat_id=" + chatID + "&text=" + message;
  
  String request = "POST " + url + " HTTP/1.1\r\n";
  request += "Host: api.telegram.org\r\n";
  request += "Content-Type: application/x-www-form-urlencoded\r\n";
  request += "Content-Length: " + String(postData.length()) + "\r\n";
  request += "Connection: close\r\n\r\n";
  request += postData;
  
  client.print(request);
  
  // Wait for response
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Telegram timeout");
      client.stop();
      return false;
    }
  }
  
  // Read response (optional)
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  client.stop();
  Serial.println("Telegram message sent: " + message);
  return true;
}

bool TelegramManager::sendAlarm(String alarmType, String details) {
  String message = "🚨 **SMART HOME ALARM** 🚨\n";
  message += "**Type:** " + alarmType + "\n";
  message += "**Details:** " + details + "\n";
  message += "**Time:** " + String(millis()/1000) + "s";
  
  return sendMessage(message);
}