#include <Arduino.h>

#include "wifi_manager.h"
#include "esp_server_portal.h"
#include "recieve_send_data.h"
#include "read_serial.h"
#include "helper_function.h"
#include "send_googlesheet.h"

const int ledPin = D0;
const int interval = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
long unsigned int previousMillis = 0;

void setup() {
  Serial.begin(9600);
  setupEEPROM();  
  setup_read_serial();
  setup_wifi_manager();  
  setup_esp_server();
  setup_sendData();
  Serial.println("Server Start");
  setupLED(ledPin);
  powerOnLED(ledPin); 
  delay(5000);
}

void loop() { 
  handle_esp_server();
  readSerialData();
  String intervalhour = readEEPROM(0);
  long unsigned int intervalhourResult = intervalhour.toInt();
  long unsigned int intervalTotal = interval * intervalhourResult;

  if (!intervalhourResult || intervalhourResult==1){
    intervalTotal = 60000;
  }

  long unsigned int currentMillis = millis();
  if (currentMillis - previousMillis >= intervalTotal) {
    handle_sendData(); 
    powerOnLED(ledPin);
    delay(100);
    powerOffLED(ledPin);
    delay(100);
    powerOnLED(ledPin);
    previousMillis = currentMillis;
  }  
}