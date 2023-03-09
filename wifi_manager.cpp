#include <Arduino.h>
#include <WiFiManager.h>


WiFiManager wifiManager;
int status = WL_IDLE_STATUS;

void ConnectWifiManager(){
  if (!wifiManager.autoConnect("AquamoniaOS","password")) {
    Serial.println("Failed to connect to WiFi and hit timeout");
    delay(5000);
  }  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
  }
}

void wmresetsetting(){
  wifiManager.resetSettings();
}

void setup_wifi_manager(){
  ConnectWifiManager();
}

void read_wifi_manager(){
}

