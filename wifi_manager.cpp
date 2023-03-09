#include <Arduino.h>
#include <WiFiManager.h>
#include <EEPROM.h>

int status = WL_IDLE_STATUS;

void ConnectWifiManager(){
  if (!wifiManager.autoConnect("AutoConnectAP","password")) {
    Serial.println("Failed to connect to WiFi and hit timeout");
    delay(5000);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
  }
}

void setup_wifi_manager(){
  ConnectWifiManager();
}

void read_wifi_manager(){
}

void resetEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM resetted.");
}

void resetingEeprom(){
  resetEEPROM();
  wifiManager.resetSettings();
}

void questions(){
  Serial.println("1. Read");
  Serial.println("2. Reset");  
  Serial.println("Which sensor would you like to read? ");
  while (Serial.available() == 0) {
  }
  int menuChoice = Serial.parseInt();
  switch (menuChoice) {
    case 1:
    {     

    }
    break;  
    case 2:
    {
      resetEEPROM();
      wifiManager.resetSettings();
    }
    break;
    default:
    {
      Serial.println("Please choose a valid selection");
    }
  }
}