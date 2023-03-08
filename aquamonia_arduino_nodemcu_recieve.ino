#include <Arduino.h>
#include <ArduinoJson.h>              // import DHT11 sensor
#include <WiFiManager.h>
#include <ESP8266WiFi.h>        //import ESP8266 WiFi library
#include <ESP8266HTTPClient.h>  //import ESP8266 HTTP Client library
#include <ESP8266WebServer.h>
#include <WiFiClient.h>   
#include <SPI.h>   // Add Wifi Client
#include <EEPROM.h>

#include "sr01_dht11_function.h"
#include "sr02_tds_function.h"
#include "sr03_turbidity_function.h"
#include "sr04_water_temp_function.h"
#include "sr05_ph_function.h"

WiFiManager wifiManager;
ESP8266WebServer server(80);
const int interval = 60000; // Waktu interval dalam milidetik (3 jam = 3 x 60 x 60 x 1000)
unsigned long previousMillis = 0;

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

char serverName[250];
char deviceKey[250];
char secretKey[1500];
char deviceKey_2[250];
char deviceKey_3[250];
char deviceKey_4[250];
char deviceKey_5[250];
char deviceKey_6[250];

WiFiManagerParameter custom_name_server("serverName", "API Endpoint URL", serverName, 250);
WiFiManagerParameter custom_secret_key("secretKey", "Secret Key", secretKey, 1500);
WiFiManagerParameter custom_device_key("deviceKey", "Device Key 1", deviceKey, 250);
WiFiManagerParameter custom_device_key_2("deviceKey_2", "Device Key 2", deviceKey_2, 250);
WiFiManagerParameter custom_device_key_3("deviceKey_3", "Device Key 3", deviceKey_3, 250);
WiFiManagerParameter custom_device_key_4("deviceKey_4", "Device Key 4", deviceKey_4, 250);
WiFiManagerParameter custom_device_key_5("deviceKey_5", "Device Key 5", deviceKey_5, 250);
WiFiManagerParameter custom_device_key_6("deviceKey_6", "Device Key 6", deviceKey_6, 250);

int status = WL_IDLE_STATUS;
WiFiClient client;

//Check if header is present and correct
bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Wrong username/password! try again.";
  Serial.println("Log in Failed");
  }

  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body class='body'>";
  html += "<div class='container text-center'>";
  html += "<form class='form-signin' action='/login' method='POST'> <p class='mb-3 font-weight-normal'>To log in, please use : admin/admin</p> <label for='inputEmail' class='sr-only'>User Name</label> <input type='text' id='USERNAME' name='USERNAME' class='form-control' placeholder='User Name' required autofocus> <label for='inputPassword' class='sr-only'>Password</label> <input type='password' id='PASSWORD' name='PASSWORD' class='form-control' placeholder='Password' required> <div class='checkbox mb-3'> <label> <input type='checkbox' value='remember-me'> Remember me </label> </div><button class='btn btn-lg btn-primary btn-block' type='submit'>Sign in</button> <p class='mt-5 mb-3 text-muted'>&copy; Aquamonia 2023</p></form>";
  html += "<p>" + msg + "</p>";
  html += "</div>";
  html += "</body>";
  html += "</html>";  
  
  server.send(200, "text/html", html);
}

//no need authentification
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleRoot() {
  server.send(200, "text/plain", "Hello from Aquamonia!");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(2048);
  setup_dht();
  setup_tds();
  setup_turbidity();
  setup_water_temp();
  setup_ph();
  wifiManager.addParameter(&custom_name_server);
  wifiManager.addParameter(&custom_secret_key);
  wifiManager.addParameter(&custom_device_key);
  wifiManager.addParameter(&custom_device_key_2);
  wifiManager.addParameter(&custom_device_key_3);
  wifiManager.addParameter(&custom_device_key_4);
  wifiManager.addParameter(&custom_device_key_5);
  wifiManager.addParameter(&custom_device_key_6);
  ConnectWifiManager();

  // inisialisasi server web dan menangani permintaan root
  server.on("/", home);
  server.on("/login", handleLogin);
  server.on("/device-setup", deviceSetup);
  server.on("/device-monitor", deviceMonitor);
  server.on("/menu-reset", menureset);
  server.on("/reseting-eeprom", resetingEeprom);
  server.on("/save-eeprom", saveWebEeProm);

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );

  server.begin();

  delay(5000);
}

void loop() {
  float temperature_sensor = readTemperature();
  float humidity_sensor = readHumidity();
  float tds_sensor = read_tds_value();
  float turbidity_sensor = read_turbidity_value();
  float water_temp_sensor = read_water_temp_value();
  float ph_sensor = read_ph_return();

  Serial.print("Temperature Reading: ");
  Serial.println(temperature_sensor);
  Serial.print("Humidity Sensor: ");
  Serial.println(humidity_sensor);
  Serial.print("TDS Sensor: ");
  Serial.println(tds_sensor);
  Serial.print("Turbidity Sensor: ");
  Serial.println(turbidity_sensor);
  Serial.print("Water Temperature Sensor: ");
  Serial.println(water_temp_sensor);
  Serial.print("PH: ");
  Serial.println(ph_sensor);
  delay(10000);

  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    sendData();
    previousMillis = currentMillis;
  }
}

void ConnectWifiManager(){
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("AutoConnectAP","password")) {
    Serial.println("Failed to connect to WiFi and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
  }
  
}

void saveConfigCallback () {
  if (WiFi.status() == WL_CONNECTED) {
      strcpy(serverName, custom_name_server.getValue());
      strcpy(secretKey, custom_secret_key.getValue());
      strcpy(deviceKey, custom_device_key.getValue());
      strcpy(deviceKey_2, custom_device_key_2.getValue());
      strcpy(deviceKey_3, custom_device_key_3.getValue());
      strcpy(deviceKey_4, custom_device_key_4.getValue());
      strcpy(deviceKey_5, custom_device_key_5.getValue());
      strcpy(deviceKey_6, custom_device_key_6.getValue());

      String EndpointUrl = String(serverName);
      String AuthSecretKey = String(secretKey);
      String DeviceApiKey = String(deviceKey);
      String DeviceApiKey_2 = String(deviceKey_2);
      String DeviceApiKey_3 = String(deviceKey_3);
      String DeviceApiKey_4 = String(deviceKey_4);
      String DeviceApiKey_5 = String(deviceKey_5);
      String DeviceApiKey_6 = String(deviceKey_6);

      int addr = 0; // alamat awal penyimpanan di EEPROM
      simpanKeEEPROM(addr, EndpointUrl);
      addr += EndpointUrl.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, AuthSecretKey);
      addr += AuthSecretKey.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey);
      addr += DeviceApiKey.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_2);
      addr += DeviceApiKey_2.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_3);
      addr += DeviceApiKey_3.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_4);
      addr += DeviceApiKey_4.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_5);
      addr += DeviceApiKey_5.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_6);
  }
  else {
    Serial.println("Failed to save config: WiFi not connected");
  }
}

void simpanKeEEPROM(int addr, String data) {
  int len = data.length();
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + len, '\0'); // tambahkan null terminator
  EEPROM.commit();
}

String bacaDariEEPROM(int addr) {
  String data = "";
  char ch = EEPROM.read(addr);
  while (ch != '\0') {
    data += ch;
    addr++;
    ch = EEPROM.read(addr);
  }
  return data;
}

void resetEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM resetted.");
}

void resetingEeprom(){
  resetEEPROM();
  wifiManager.resetSettings();
  server.stop();
}

void questions(){
  Serial.println("1. Read");
  Serial.println("2. Reset");  
  Serial.println("Which sensor would you like to read? ");
  while (Serial.available() == 0) {
  }
  int menuChoice = Serial.parseInt();
  switch (menuChoice) {
    case 1:
    {     
      sendData();
    }
    break;  
    case 2:
    {
      resetEEPROM();
      wifiManager.resetSettings();
    }
    break;
    default:
    {
      Serial.println("Please choose a valid selection");
    }
  }
}

void sendData() {
  float value = random(24.1,30.9);
  float value_device_2 = random(70.1,90.9);
  float value_device_3 = random(300,500);
  float value_device_4 = random(30,70);
  float value_device_5 = random(20.1,35.9);
  float value_device_6 = random(6.1,8.1);

  String NewEndpointUrl = bacaDariEEPROM(0);
  String NewAuthSecretKey = bacaDariEEPROM(NewEndpointUrl.length() + 1);
  String NewDeviceApiKey = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + 2);
  String NewDeviceApiKey_2 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + 3);
  String NewDeviceApiKey_3 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + 4);
  String NewDeviceApiKey_4 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + 5);
  String NewDeviceApiKey_5 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + 6);          
  String NewDeviceApiKey_6 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + NewDeviceApiKey_5.length() + 7);          
  
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

void sendFormData(String EndpointUrl, String AuthSecretKey, String DeviceApiKey, float value, String measurement) {
  WiFiClient client;
  HTTPClient http;
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

void login () {
  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<header> <nav class='navbar navbar-expand-md navbar-light fixed-top' style='background-color: #e3f2fd;'> <div class='container'> <a class='navbar-brand' href='/'>Aquamonia Devices Setup</a> <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarCollapse' aria-controls='navbarCollapse' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span> </button> <div class='collapse navbar-collapse' id='navbarCollapse'> <ul class='navbar-nav ml-auto'> <li class='nav-item active'> <a class='nav-link' href='/'>Home</a> </li><li class='nav-item'> <a class='nav-link' href='/device-setup'>Setup Device API's</a> </li><li class='nav-item'> <a class='nav-link' href='/device-monitor'>Device Monitor</a> </li><li class='nav-item'> <a class='nav-link' href='https://aquamonia.com' target='_blank'>OS Aquamonia</a> </li></ul> </div></div></nav> </header>";
  html += "<main role='main' style='padding-top: 100px'> <header class='text-center'> <h1Login</h1> </header></main>";
  html += "<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js'></script>";
  html += "</div>";
  html += "</body>";
  html += "</html>";  
  
  server.send(200, "text/html", html);
}

void home() {
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<header> <nav class='navbar navbar-expand-md navbar-light fixed-top' style='background-color: #e3f2fd;'> <div class='container'> <a class='navbar-brand' href='/'>Aquamonia Devices Setup</a> <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarCollapse' aria-controls='navbarCollapse' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span> </button> <div class='collapse navbar-collapse' id='navbarCollapse'> <ul class='navbar-nav ml-auto'> <li class='nav-item active'> <a class='nav-link' href='/'>Home</a> </li><li class='nav-item'> <a class='nav-link' href='/device-setup'>Setup Device API's</a> </li><li class='nav-item'> <a class='nav-link' href='/device-monitor'>Device Monitor</a> </li><li class='nav-item'> <a class='nav-link' href='https://aquamonia.com' target='_blank'>OS Aquamonia</a> </li><li class='nav-item'> <a class='nav-link' href='/menu-reset'>Reset Config</a> </li><li class='nav-item'> <a class='nav-link' href=\"/login?DISCONNECT=YES\">Logout</a> </li></ul> </div></div></nav> </header>";
  html += "<main role='main' style='padding-top: 100px'> <header class='text-center'> <h1>Home</h1> </header></main>";
  html += "<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js'></script>";
  html += "</div>";
  html += "</body>";
  html += "</html>";  

  server.send(200, "text/html", html);
}

void deviceMonitor() {
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }

  float value = random(24.1,30.9);
  float value_device_2 = random(70.1,90.9);
  float value_device_3 = random(300,500);
  float value_device_4 = random(30,70);
  float value_device_5 = random(20.1,35.9);
  float value_device_6 = random(6.1,8.1);
  
  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<header> <nav class='navbar navbar-expand-md navbar-light fixed-top' style='background-color: #e3f2fd;'> <div class='container'> <a class='navbar-brand' href='/'>Aquamonia Devices Setup</a> <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarCollapse' aria-controls='navbarCollapse' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span> </button> <div class='collapse navbar-collapse' id='navbarCollapse'> <ul class='navbar-nav ml-auto'> <li class='nav-item'> <a class='nav-link' href='/'>Home</a> </li><li class='nav-item'> <a class='nav-link' href='/device-setup'>Setup Device API's</a> </li><li class='nav-item active'> <a class='nav-link' href='/device-monitor'>Device Monitor</a> </li><li class='nav-item'> <a class='nav-link' href='https://aquamonia.com' target='_blank'>OS Aquamonia</a> </li><li class='nav-item'> <a class='nav-link' href='/menu-reset'>Reset Config</a> </li><li class='nav-item'> <a class='nav-link' href=\"/login?DISCONNECT=YES\">Logout</a> </li></ul> </div></div></nav> </header>";
  html += "<main role='main' style='padding-top: 100px'> <header class='text-center'> <p>This Page will be refresh in <span id='time'>32</span> second</p><h1>Device Monitoring</h1> </header> <div class='row text-center'> <div class='col-12'></div><div id='gg2' class='col-4 gauge' data-value='" + String(value) +"' data-title='Temperature Sensor'></div><div id='gg3' class='col-4 gauge' data-value='" + String(value_device_2) +"' data-title='Humidity Sensor'></div><div id='gg4' class='col-4 gauge' data-value='" + String(value_device_3) +"' data-title='TDS Sensor'></div><div id='gg5' class='col-4 gauge' data-value='" + String(value_device_4) +"' data-title='Turbidity Sensor'></div><div id='gg6' class='col-4 gauge' data-value='" + String(value_device_5) +"' data-title='Water Temperature'></div><div id='gg7' class='col-4 gauge' data-value='" + String(value_device_6) +"' data-title='PH Water Sensor'></div></div></main>";
  html += "<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js'></script> <script src='https://aquamonia.com/arduino/assets/js/raphael.min.js'></script> <script src='https://aquamonia.com/arduino/assets/js/justgage.js'></script> <script src='https://aquamonia.com/arduino/assets/js/script.js'></script>";
  html += "</div>";
  html += "</body>";
  html += "</html>";  
  server.send(200, "text/html", html);
}

void deviceSetup(){
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }

  String NewEndpointUrl = bacaDariEEPROM(0);
  String NewAuthSecretKey = bacaDariEEPROM(NewEndpointUrl.length() + 1);
  String NewDeviceApiKey = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + 2);
  String NewDeviceApiKey_2 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + 3);
  String NewDeviceApiKey_3 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + 4);
  String NewDeviceApiKey_4 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + 5);
  String NewDeviceApiKey_5 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + 6);          
  String NewDeviceApiKey_6 = bacaDariEEPROM(NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + NewDeviceApiKey_5.length() + 7);          
  
  String EndpointUrl = String(NewEndpointUrl);
  String AuthSecretKey = String(NewAuthSecretKey);
  String DeviceApiKey = String(NewDeviceApiKey);
  String DeviceApiKey_2 = String(NewDeviceApiKey_2);
  String DeviceApiKey_3 = String(NewDeviceApiKey_3);
  String DeviceApiKey_4 = String(NewDeviceApiKey_4);
  String DeviceApiKey_5 = String(NewDeviceApiKey_5);
  String DeviceApiKey_6 = String(NewDeviceApiKey_6);

  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<header> <nav class='navbar navbar-expand-md navbar-light fixed-top' style='background-color: #e3f2fd;'> <div class='container'> <a class='navbar-brand' href='/'>Aquamonia Devices Setup</a> <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarCollapse' aria-controls='navbarCollapse' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span> </button> <div class='collapse navbar-collapse' id='navbarCollapse'> <ul class='navbar-nav ml-auto'> <li class='nav-item'> <a class='nav-link' href='/'>Home</a> </li><li class='nav-item active'> <a class='nav-link' href='/device-setup'>Setup Device API's</a> </li><li class='nav-item'> <a class='nav-link' href='/device-monitor'>Device Monitor</a> </li><li class='nav-item'> <a class='nav-link' href='https://aquamonia.com' target='_blank'>OS Aquamonia</a> </li><li class='nav-item'> <a class='nav-link' href='/menu-reset'>Reset Config</a> </li><li class='nav-item'> <a class='nav-link' href=\"/login?DISCONNECT=YES\">Logout</a> </li></ul> </div></div></nav> </header>";
  html += "<main role='main' style='margin-top: 100px;margin-bottom: 50px;'> <header class='text-center'><h1 style='margin-bottom: 50px;'>Device API Key Setup</h1> </header>";

  html += "<div class='table-responsive' style='margin-bottom: 50px'>";
  html += "<form method='POST' action='/save-eeprom'>";
  html += "<table class='table table-striped'>";
  html += "<thead>";
  html += "<tr>";
  html += "<th>#</th>";
  html += "<th>Token Name</th>";
  html += "<th>Token Secret Key</th>";
  html += "</tr>";
  html += "</thead>";
  html += "<tbody>";
  html += "<tr>";
  html += "<th scope='row'>1</th>";
  html += "<td>Endpoint API Url</td>";
  html += "<td><input type='text' id='enpoint' name='serverName' value='" + EndpointUrl + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>1</th>";
  html += "<td>Secret Key</td>";
  html += "<td><input type='text' id='token' name='secretKey' value='" + AuthSecretKey + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "</tbody>";
  html += "</table>";
  html += "</div>";

  html += "<div class='table-responsive'>";  
  html += "<table class='table table-striped'>";
  html += "<thead>";
  html += "<tr>";
  html += "<th>#</th>";
  html += "<th>Device Number</th>";
  html += "<th>Device API Key</th>";
  html += "<th>Action</th>";
  html += "</tr>";
  html += "</thead>";
  html += "<tbody>";
  html += "<tr>";
  html += "<th scope='row'>1</th>";
  html += "<td>Device 1</td>";
  html += "<td>" + DeviceApiKey + "</td>";
  html += "<td><input type='text' id='device_1' name='deviceKey' value='" + DeviceApiKey + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>2</th>";
  html += "<td>Device 2</td>";
  html += "<td>" + DeviceApiKey_2 + "</td>";
  html += "<td><input type='text' id='device_2' name='deviceKey_2' value='" + DeviceApiKey_2 + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>3</th>";
  html += "<td>Device 3</td>";
  html += "<td>" + DeviceApiKey_3 + "</td>";
  html += "<td><input type='text' id='device_3' name='deviceKey_3' value='" + DeviceApiKey_3 + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>4</th>";
  html += "<td>Device 4</td>";
  html += "<td>" + DeviceApiKey_4 + "</td>";
  html += "<td><input type='text' id='device_4' name='deviceKey_4' value='" + DeviceApiKey_4 + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>5</th>";
  html += "<td>Device 5</td>";
  html += "<td>" + DeviceApiKey_5 + "</td>";
  html += "<td><input type='text' id='device_5' name='deviceKey_5' value='" + DeviceApiKey_5 + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>6</th>";
  html += "<td>Device 6</td>";
  html += "<td>" + DeviceApiKey_6 + "</td>";
  html += "<td><input type='text' id='device_6' name='deviceKey_6' value='" + DeviceApiKey_6 + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "</tbody>";
  html += "</table>";
  html += "<div class='text-center'><button type='submit' class='btn btn-primary'>Save Configuration</button></div>";
  html += "</form>";
  html += "</div>";
  html += "</main>";
  html += "<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js'></script>";
  html += "</div>";
  html += "</body>";
  html += "</html>";    
  server.send(200, "text/html", html);
}

void menureset(){
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<header> <nav class='navbar navbar-expand-md navbar-light fixed-top' style='background-color: #e3f2fd;'> <div class='container'> <a class='navbar-brand' href='/'>Aquamonia Devices Setup</a> <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarCollapse' aria-controls='navbarCollapse' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span> </button> <div class='collapse navbar-collapse' id='navbarCollapse'> <ul class='navbar-nav ml-auto'> <li class='nav-item'> <a class='nav-link' href='/'>Home</a> </li><li class='nav-item'> <a class='nav-link' href='/device-setup'>Setup Device API's</a> </li><li class='nav-item'> <a class='nav-link' href='/device-monitor'>Device Monitor</a> </li><li class='nav-item'> <a class='nav-link' href='https://aquamonia.com' target='_blank'>OS Aquamonia</a> </li><li class='nav-item active'> <a class='nav-link' href='/menu-reset'>Reset Config</a> </li><li class='nav-item'> <a class='nav-link' href=\"/login?DISCONNECT=YES\">Logout</a> </li></ul> </div></div></nav> </header>";
  html += "<main role='main' style='padding-top: 100px'> <header class='text-center'> <h1>Reset Menu</h1> </header>";
  html += "<div class='text-center'><a class='btn btn-primary' href='/reseting-eeprom'>Reset Configuration</a></div>";  
  html += "</main>";
  html += "<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js'></script> <script src='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js'></script>";
  html += "</div>";
  html += "</body>";
  html += "</html>";  

  server.send(200, "text/html", html);
}

void saveWebEeProm(){
      String EndpointUrl = server.arg("serverName");
      String AuthSecretKey = server.arg("secretKey");
      String DeviceApiKey = server.arg("deviceKey");
      String DeviceApiKey_2 = server.arg("deviceKey_2");
      String DeviceApiKey_3 = server.arg("deviceKey_3");
      String DeviceApiKey_4 = server.arg("deviceKey_4");
      String DeviceApiKey_5 = server.arg("deviceKey_5");
      String DeviceApiKey_6 = server.arg("deviceKey_6");

      int addr = 0; // alamat awal penyimpanan di EEPROM
      simpanKeEEPROM(addr, EndpointUrl);
      addr += EndpointUrl.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, AuthSecretKey);
      addr += AuthSecretKey.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey);
      addr += DeviceApiKey.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_2);
      addr += DeviceApiKey_2.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_3);
      addr += DeviceApiKey_3.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_4);
      addr += DeviceApiKey_4.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_5);
      addr += DeviceApiKey_5.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      simpanKeEEPROM(addr, DeviceApiKey_6);

      server.sendHeader("Location","/device-setup");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
}