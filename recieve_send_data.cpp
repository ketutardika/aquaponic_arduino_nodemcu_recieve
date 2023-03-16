#include <Arduino.h>
#include <ESP8266WiFi.h>        //import ESP8266 WiFi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include "read_serial.h"
#include "helper_function.h"


WiFiClient client;
HTTPClient http;

String measurement = "C";
String measurement_2 = "%";
String measurement_3 = "PPM";
String measurement_4 = "NTU";
String measurement_5 = "C";
String measurement_6 = "PH";

void sendFormData(String EndpointUrl, String AuthSecretKey, String DeviceApiKey, String value, String measurement) {
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

void setup_sendData(){
  setupEEPROM();
}

void handle_sendData() {  
  String value_devices_1 = readEEPROM(4000);
  String value_devices_2 = readEEPROM(4000 + value_devices_1.length() + 1);
  String value_devices_3 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + 2);
  String value_devices_4 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + 3);
  String value_devices_5 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + 4);
  String value_devices_6 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + value_devices_5.length() + 5);

  String NewSendInterval = readEEPROM(0);
  String NewEndpointUrl = readEEPROM(NewSendInterval.length() + 1);
  String NewAuthSecretKey = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + 2);
  String NewDeviceApiKey  = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + 3);
  String NewDeviceApiKey_2 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + 4);
  String NewDeviceApiKey_3 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + 5);
  String NewDeviceApiKey_4 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + 6);
  String NewDeviceApiKey_5 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + 7);          
  String NewDeviceApiKey_6 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + NewDeviceApiKey_5.length() + 8);          
  
  String LenSendInterval = String(NewSendInterval);
  String LenEndpointUrl = String(NewEndpointUrl);
  String LenAuthSecretKey = String(NewAuthSecretKey);
  String LenDeviceApiKey = String(NewDeviceApiKey);
  String LenDeviceApiKey_2 = String(NewDeviceApiKey_2);
  String LenDeviceApiKey_3 = String(NewDeviceApiKey_3);
  String LenDeviceApiKey_4 = String(NewDeviceApiKey_4);
  String LenDeviceApiKey_5 = String(NewDeviceApiKey_5);
  String LenDeviceApiKey_6 = String(NewDeviceApiKey_6);

  String SendInterval = LenSendInterval.length() > 0 ? LenSendInterval : "0";
  String EndpointUrl = LenEndpointUrl.length() > 0 ? LenEndpointUrl : "http://aquamonia.com";
  String AuthSecretKey = LenAuthSecretKey.length() > 0 ? LenAuthSecretKey : "0";
  String DeviceApiKey  = LenDeviceApiKey.length() > 0 ? LenDeviceApiKey : "0";
  String DeviceApiKey_2 = LenDeviceApiKey_2.length() > 0 ? LenDeviceApiKey_2 : "0";
  String DeviceApiKey_3 = LenDeviceApiKey_3.length() > 0 ? LenDeviceApiKey_3 : "0";
  String DeviceApiKey_4 = LenDeviceApiKey_4.length() > 0 ? LenDeviceApiKey_4 : "0";
  String DeviceApiKey_5 = LenDeviceApiKey_5.length() > 0 ? LenDeviceApiKey_5 : "0";
  String DeviceApiKey_6 = LenDeviceApiKey_6.length() > 0 ? LenDeviceApiKey_6 : "0";

  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey, value_devices_1, measurement);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_2, value_devices_2, measurement_2);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_3, value_devices_3, measurement_3);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_4, value_devices_4, measurement_4);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_5, value_devices_5, measurement_5);
  sendFormData(EndpointUrl, AuthSecretKey, DeviceApiKey_6, value_devices_6, measurement_6);
}