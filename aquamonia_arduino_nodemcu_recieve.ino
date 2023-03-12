#include <Arduino.h>
#include <ArduinoJson.h> 
#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "wifi_manager.h"
#include "esp_server_portal.h"
#include "recieve_send_data.h"

SoftwareSerial SerialMega(D6, D5);

const int interval = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  SerialMega.begin(9600);
  Serial.println("Setup");
  setup_wifi_manager();
}

void loop() { 
  readSerials();
}

void readSerials() {
  //Menunggu sampai ada data yang diterima dari Arduino
  while (!SerialMega.available()) {
    delay(100);
  }
  // Membaca data dari Arduino
  String jsonString = "";
  while (SerialMega.available()) {
    jsonString += (char)SerialMega.read();
  }
  //Parsing data JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, SerialMega);
  if (!error){
    //Mengambil nilai dari objek JSON
    String tds = doc["tds_sensor"].as<String>();
    String ph = doc["ph_sensor"].as<String>();

    float temperature_sensor = doc["temperature_sensor"].as<float>();
    float humidity_sensor = doc["humidity_sensor"].as<float>();
    float tds_sensor = doc["tds_sensor"].as<float>();
    float turbidity_sensor = doc["turbidity_sensor"].as<float>();
    float water_temp_sensor = doc["water_temp_sensor"].as<float>();
    float ph_sensor = doc["ph_sensor"].as<float>();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println(temperature_sensor);
      Serial.println(humidity_sensor);
      Serial.println(tds_sensor);
      Serial.println(turbidity_sensor);
      Serial.println(water_temp_sensor);
      Serial.println(ph_sensor);
      previousMillis = currentMillis;
    }

    
  }
}