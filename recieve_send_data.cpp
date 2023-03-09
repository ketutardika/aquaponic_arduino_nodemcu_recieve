#include <Arduino.h>
#include <ArduinoJson.h> 
#include <ESP8266WiFi.h>        //import ESP8266 WiFi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>

WiFiClient client;
HTTPClient http;

float value = 0;
float value_device_2 = 0;
float value_device_3 = 0;
float value_device_4 = 0;
float value_device_5 = 0;
float value_device_6 = 0;

String measurement = "C";
String measurement_2 = "%";
String measurement_3 = "PPM";
String measurement_4 = "NTU";
String measurement_5 = "C";
String measurement_6 = "PH";

String bacaDariEEPROMMain(int addr) {
  String data = "";
  char ch = EEPROM.read(addr);
  while (ch != '\0') {
    data += ch;
    addr++;
    ch = EEPROM.read(addr);
  }
  return data;
}

void sendFormData(String EndpointUrl, String AuthSecretKey, String DeviceApiKey, float value, String measurement) {
  http.begin(client, EndpointUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Accept", "application/json");
  http.addHeader("Authorization", "Bearer " + String(AuthSecretKey));
  String requestBody = "unique_id=" + String(DeviceApiKey) + "&value=" + String(value) + "&measurement=" + String(measurement);
  int httpResponseCode = http.POST(requestBody);
  String response = http.getString();
  Serial.println("HTTP Response code: " + String(httpResponseCode));
  http.end();  
}

void handle_sendData() {
  EEPROM.begin(2048);
  float value = random(24.1,30.9);
  float value_device_2 = random(70.1,90.9);
  float value_device_3 = random(300,500);
  float value_device_4 = random(30,70);
  float value_device_5 = random(20.1,35.9);
  float value_device_6 = random(6.1,8.1);

  String NewEndpointUrl = bacaDariEEPROMMain(0);
  String NewAuthSecretKey = bacaDariEEPROMMain(NewEndpointUrl.length() + 1);
  String NewDeviceApiKey = bacaDariEEPROMMain(NewEndpointUrl.length() + NewAuthSecretKey.length() + 2);
  String NewDeviceApiKey_2 = bacaDariEEPROMMain(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + 3);
  String NewDeviceApiKey_3 = bacaDariEEPROMMain(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + 4);
  String NewDeviceApiKey_4 = bacaDariEEPROMMain(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + 5);
  String NewDeviceApiKey_5 = bacaDariEEPROMMain(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + 6);          
  String NewDeviceApiKey_6 = bacaDariEEPROMMain(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + NewDeviceApiKey_5.length() + 7);          
  
  String LenEndpointUrl = String(NewEndpointUrl);
  String LenAuthSecretKey = String(NewAuthSecretKey);
  String LenDeviceApiKey = String(NewDeviceApiKey);
  String LenDeviceApiKey_2 = String(NewDeviceApiKey_2);
  String LenDeviceApiKey_3 = String(NewDeviceApiKey_3);
  String LenDeviceApiKey_4 = String(NewDeviceApiKey_4);
  String LenDeviceApiKey_5 = String(NewDeviceApiKey_5);
  String LenDeviceApiKey_6 = String(NewDeviceApiKey_6);

  String EndpointUrl = LenEndpointUrl.length() > 0 ? LenEndpointUrl : "http://aquamonia.com";
  String AuthSecretKey = LenAuthSecretKey.length() > 0 ? LenAuthSecretKey : "0";
  String DeviceApiKey  = LenDeviceApiKey.length() > 0 ? LenDeviceApiKey : "0";
  String DeviceApiKey_2 = LenDeviceApiKey_2.length() > 0 ? LenDeviceApiKey_2 : "0";
  String DeviceApiKey_3 = LenDeviceApiKey_3.length() > 0 ? LenDeviceApiKey_3 : "0";
  String DeviceApiKey_4 = LenDeviceApiKey_4.length() > 0 ? LenDeviceApiKey_4 : "0";
  String DeviceApiKey_5 = LenDeviceApiKey_5.length() > 0 ? LenDeviceApiKey_5 : "0";
  String DeviceApiKey_6 = LenDeviceApiKey_6.length() > 0 ? LenDeviceApiKey_6 : "0";

  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey, value, measurement);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_2, value_device_2, measurement_2);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_3, value_device_3, measurement_3);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_4, value_device_4, measurement_4);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_5, value_device_5, measurement_5);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_6, value_device_6, measurement_6);
}