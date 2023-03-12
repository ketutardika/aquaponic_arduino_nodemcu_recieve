#include <WiFiManager.h>

WiFiManager wifiManager;

void ConnectWifiManager(){ 

  bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wifiManager.autoConnect("AquamoniaOS","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("wifi connected");
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

