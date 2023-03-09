#include <Arduino.h>
#include <ArduinoJson.h> 

#include "wifi_manager.h"
#include "esp_server_portal.h"
#include "recieve_send_data.h"

const int interval = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi_manager();
  setup_esp_server();
  delay(5000);
}

void loop() { 
  handle_esp_server();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    handle_sendData();
    previousMillis = currentMillis;
  }
}


