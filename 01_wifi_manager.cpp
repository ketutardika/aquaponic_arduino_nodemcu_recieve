#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h> 
WiFiManager wifiManager;

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