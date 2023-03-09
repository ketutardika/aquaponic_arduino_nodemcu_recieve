#include <Arduino.h>
#include <ArduinoJson.h> 
#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "wifi_manager.h"
#include "esp_server_portal.h"
#include "recieve_send_data.h"

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

float temperature_sensor, humidity_sensor, tds_sensor, turbidity_sensor, water_temp_sensor, ph_sensor;

const int interval = 3600000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  setup_sendData();
  Serial1.begin(9600);
  nodemcu.begin(9600);
  setup_wifi_manager();
  setup_esp_server();
  delay(5000);
}

String bacaDariEEPROMMainMaster(int addr) {
  String data = "";
  char ch = EEPROM.read(addr);
  while (ch != '\0') {
    data += ch;
    addr++;
    ch = EEPROM.read(addr);
  }
  return data;
}

void loop() { 
  handle_esp_server();
  if (nodemcu.available()) 
  {
  StaticJsonDocument<300> doc;
  DeserializationError error = deserializeJson(doc, nodemcu);
  // Check for errors
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  float temperature_sensor = doc["temperature_sensor"];
  float humidity_sensor = doc["humidity_sensor"];
  float tds_sensor = doc["tds_sensor"];
  float turbidity_sensor = doc["turbidity_sensor"];
  float water_temp_sensor = doc["water_temp_sensor"];
  float ph_sensor = doc["ph_sensor"];

  Serial.println(temperature_sensor);
  Serial.println(humidity_sensor);
  Serial.println(tds_sensor);
  Serial.println(turbidity_sensor);
  Serial.println(water_temp_sensor);
  Serial.println(ph_sensor);
  delay(10000);
  } else {
    Serial.println("Node Not Available");
    delay(10000);
  }
  
  String intervalhour = bacaDariEEPROMMainMaster(0);
  int intervalhourResult = intervalhour.toInt();
  int intervalTotal = interval * intervalhourResult;

  if (!intervalhourResult || intervalhourResult==1){
    intervalTotal = 60000;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalTotal) {
    handle_sendData();
    previousMillis = currentMillis;
  }
}