#include <Arduino.h>
#include <ArduinoJson.h> 
#include <EEPROM.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "wifi_manager.h"
#include "esp_server_portal.h"
#include "recieve_send_data.h"
#include "read_serial.h"

const int interval = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
long unsigned int previousMillis = 0;

void setup() {
  Serial.begin(9600);
  setup_read_serial();
  setup_wifi_manager();
  setup_esp_server();
  setup_sendData();
  Serial.println("Server Start");
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
  String intervalhour = bacaDariEEPROMMainMaster(0);
  long unsigned int intervalhourResult = intervalhour.toInt();
  long unsigned int intervalTotal = interval * intervalhourResult;

  if (!intervalhourResult || intervalhourResult==1){
    intervalTotal = 60000;
  }

  long unsigned int currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    readSerials();
    handle_sendData();
    previousMillis = currentMillis;
  }
}

void readSerials() {
  float temperature_value = readTemperature();
  float humidity_value = readHumidity();
  float tds_value = readTds();
  float turbidity_value = readTurbidity();
  float water_temp_value = readWaterTemp();
  float ph_value = readPh();

  Serial.print("temp : ");
  Serial.println(temperature_value);
  Serial.print("humidity : ");
  Serial.println(humidity_value);
  Serial.print("tds : ");
  Serial.println(tds_value);
  Serial.print("turbidity : ");
  Serial.println(turbidity_value);
  Serial.print("water_temp : ");
  Serial.println(water_temp_value);
  Serial.print("ph : ");
  Serial.println(ph_value);
}

void sendSpreadSheet(){
  // Buat objek JSON dengan data sensor
  StaticJsonDocument<200> doc;
  doc["sensor1"] = 20.2;
  doc["sensor2"] = 30.3;
  doc["sensor3"] = 40.4;

  // Konversi objek JSON ke string
  String json;
  serializeJson(doc, json);

  // Kirim data ke API Endpoint
  WiFiClientSecure client2;
  client2.setInsecure();
  
  HTTPClient http2;
  http2.begin(client2, "https://script.google.com/macros/s/AKfycbyJrGSWMr2ufuN-klYQ5dq1HNC8kH6PDTA_2vm23HsDUbVRx4VfO4h6ZBcee56P-5COJw/execc");
  http2.addHeader("Content-Type", "application/json");
  int httpResponseCode = http2.POST(json);
  if (httpResponseCode == 200) {
    Serial.println("Data berhasil dikirim");
  } else {
    Serial.print("Kesalahan HTTP: ");
    Serial.println(httpResponseCode);
  }
  http2.end();
}