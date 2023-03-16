#include <ArduinoJson.h> 
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#include "helper_function.h"

//D6 = Rx & D5 = Tx
SoftwareSerial SerialMega(D6, D5);
float temperature_sensor, humidity_sensor, tds_sensor, turbidity_sensor, water_temp_sensor, ph_sensor;

#define HOUR (0.1 * 60 * 1000L)
unsigned long last_time = 0L;

//Timer to run Arduino code every 5 seconds
unsigned long previousMillisBB = 0;
unsigned long currentMillisBB;
const unsigned long periodBB = 1000;

void setup_read_serial(){
  SerialMega.begin(9600);
  setupEEPROM();
}

void readSensor() {
  delay(20000);

  String jsonString;

  while (SerialMega.available()) {
      char c = SerialMega.read();
      jsonString += c;
      Serial.println(jsonString);
      delay(2);
    }

    if (jsonString.length()>0){
      Serial.println(jsonString);
      jsonString = "";
    }

    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, jsonString);

      Serial.println(error.c_str());

    float temperature_sensor = doc["temperature_sensor"]; // 32.6
    float humidity_sensor = doc["humidity_sensor"]; // 79
    float tds_sensor = doc["tds_sensor"]; // 528.6068
    float turbidity_sensor = doc["turbidity_sensor"]; // 0.268555
    float water_temp_sensor = doc["water_temp_sensor"]; // 27.8125
    float ph_sensor = doc["ph_sensor"]; // 7.582194
     
    Serial.println(temperature_sensor);
     Serial.println( humidity_sensor);
     Serial.println(tds_sensor);
     Serial.println(turbidity_sensor);
     Serial.println(water_temp_sensor);
     Serial.println(ph_sensor);
     
     delay(5000); 
}

void readSerialData() {
  if (SerialMega.available()) {
    String data = SerialMega.readStringUntil('\n'); // Read data from Arduino

    // Split data into separate values
    int commaIndex = data.indexOf(',');
    String temperatureva = data.substring(0, commaIndex);
    data = data.substring(commaIndex + 1);
    commaIndex = data.indexOf(',');
    String humidityva = data.substring(0, commaIndex);
    data = data.substring(commaIndex + 1);
    commaIndex = data.indexOf(',');
    String tdsva = data.substring(0, commaIndex);
    data = data.substring(commaIndex + 1);
    commaIndex = data.indexOf(',');
    String turbidityva = data.substring(0, commaIndex);
    data = data.substring(commaIndex + 1);
    commaIndex = data.indexOf(',');
    String  water_temperatureva = data.substring(0, commaIndex);
    data = data.substring(commaIndex + 1);
    commaIndex = data.indexOf(',');
    String phva = data;

    String temperature = temperatureva.length() > 0 ? temperatureva : "-1";
    String humidity = humidityva.length() > 0 ? humidityva : "-1";
    String tds =  tdsva.length() > 0 ? tdsva : "-1";
    String turbidity = turbidityva.length() > 0 ? turbidityva : "-1";
    String water_temp = water_temperatureva.length() > 0 ? water_temperatureva : "-1";
    String ph = phva.length() > 0 ? phva : "-1";


    int addr = 4000; // alamat awal penyimpanan di EEPROM
    saveEEPROM(addr, temperature);
    addr += temperature.length() + 1;
    saveEEPROM(addr, humidity);
    addr += humidity.length() + 1; // tambahkan panjang string + 1 untuk null terminator
    saveEEPROM(addr, tds);
    addr += tds.length() + 1; // tambahkan panjang string + 1 untuk null terminator
    saveEEPROM(addr, turbidity);
    addr += turbidity.length() + 1; // tambahkan panjang string + 1 untuk null terminator
    saveEEPROM(addr, water_temp);
    addr += water_temp.length() + 1; // tambahkan panjang string + 1 untuk null terminator
    saveEEPROM(addr, ph);


    IPAddress broadCast = WiFi.localIP();
    StaticJsonDocument<200> doc;
    doc["ip1"] = broadCast[0];
    doc["ip2"] = broadCast[1];
    doc["ip3"] = broadCast[2];
    doc["ip4"] = broadCast[3];
    String jsonString;
    serializeJson(doc, jsonString);
    // Mengirim data ke SerialNode
    SerialMega.println(jsonString);
    Serial.print("Json : " + jsonString);

    // Print data to serial monitor
    Serial.print(" | Recieve Data From Arduino: ");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("  Humidity: ");
    Serial.print(humidity);
    Serial.print("  TDS: ");
    Serial.print(tds);
    Serial.print("  Turbidity: ");
    Serial.print(turbidity);
    Serial.print("  Water Temperature: ");
    Serial.print(water_temp);
    Serial.print("  PH: ");
    Serial.println(ph);
  }
}

float readTemperature(){
  return 0;  
}

float readHumidity(){
  return 0;
}

float readTds(){
  return 0; 
}

float readTurbidity(){
  return 0;
}

float readWaterTemp(){
  return 0; 
}

float readPh(){
  return 0;
}