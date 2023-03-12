#include <ArduinoJson.h> 
#include <SoftwareSerial.h>

//D6 = Rx & D5 = Tx
SoftwareSerial SerialMega(D6, D5);
float temperature_sensor, humidity_sensor, tds_sensor, turbidity_sensor, water_temp_sensor, ph_sensor;
float temperature, humidity, tds, turbidity, water_temp, ph;
float suhu, kelembaban, cahaya;

void setup_read_serial(){
  SerialMega.begin(9600);
}

void readSensor() {
  if (SerialMega.available()) {
    String jsonString = "";
    while (SerialMega.available()) {
      jsonString += (char)SerialMega.read();
    }
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, SerialMega);
    if (!error){
      temperature = doc["temperature_sensor"].as<float>();
      humidity = doc["humidity_sensor"].as<float>();
      tds = doc["tds_sensor"].as<float>();
      turbidity = doc["turbidity_sensor"].as<float>();
      water_temp = doc["water_temp_sensor"].as<float>();
      ph = doc["ph_sensor"].as<float>();
    }
  } 
}

float readTemperature(){
  readSensor();
  float temperature_val = temperature;
  return temperature_val;  
}

float readHumidity(){
  readSensor();
  float humidity_val = humidity;
  return humidity_val;
}

float readTds(){
  readSensor();
  float tds_val = tds;
  return tds_val; 
}

float readTurbidity(){
  readSensor();
  float turbidity_val = turbidity;
  return turbidity_val;
}

float readWaterTemp(){
  readSensor();
  float water_temp_val = water_temp;
  return water_temp_val;
}

float readPh(){
  readSensor();
  float ph_val = ph;
  return ph_val;
}
