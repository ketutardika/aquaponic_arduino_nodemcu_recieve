#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "wifi_manager.h"
#include "read_serial.h"
#include "helper_function.h"

ESP8266WebServer server(80);

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

  String value_devices_1 = readEEPROM(4000);
  String value_devices_2 = readEEPROM(4000 + value_devices_1.length() + 1);
  String value_devices_3 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + 2);
  String value_devices_4 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + 3);
  String value_devices_5 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + 4);
  String value_devices_6 = readEEPROM(4000 + value_devices_1.length() + value_devices_2.length() + value_devices_3.length() + value_devices_4.length() + value_devices_5.length() + 5);
  
  String html = "<html charset=UTF-8>";
  html += "<head> <meta charset='utf-8'/> <meta name='viewport' content='width=device-width'> <title>Arduino Device Setup | Aquamonia</title> <meta name='title' content='Arduino Device Setup | Aquamonia'> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css'> <link rel='stylesheet' href='https://aquamonia.com/arduino/assets/css/style.css'></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<header> <nav class='navbar navbar-expand-md navbar-light fixed-top' style='background-color: #e3f2fd;'> <div class='container'> <a class='navbar-brand' href='/'>Aquamonia Devices Setup</a> <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarCollapse' aria-controls='navbarCollapse' aria-expanded='false' aria-label='Toggle navigation'> <span class='navbar-toggler-icon'></span> </button> <div class='collapse navbar-collapse' id='navbarCollapse'> <ul class='navbar-nav ml-auto'> <li class='nav-item'> <a class='nav-link' href='/'>Home</a> </li><li class='nav-item'> <a class='nav-link' href='/device-setup'>Setup Device API's</a> </li><li class='nav-item active'> <a class='nav-link' href='/device-monitor'>Device Monitor</a> </li><li class='nav-item'> <a class='nav-link' href='https://aquamonia.com' target='_blank'>OS Aquamonia</a> </li><li class='nav-item'> <a class='nav-link' href='/menu-reset'>Reset Config</a> </li><li class='nav-item'> <a class='nav-link' href=\"/login?DISCONNECT=YES\">Logout</a> </li></ul> </div></div></nav> </header>";
  html += "<main role='main' style='padding-top: 100px'> <header class='text-center'> <p>This Page will be refresh in <span id='time'>32</span> second</p><h1>Device Monitoring</h1> </header> <div class='row text-center'> <div class='col-12'></div><div id='gg2' class='col-4 gauge' data-value='" + String(value_devices_1) +"' data-title='Temperature Sensor'></div><div id='gg3' class='col-4 gauge' data-value='" + String(value_devices_2) +"' data-title='Humidity Sensor'></div><div id='gg4' class='col-4 gauge' data-value='" + String(value_devices_3) +"' data-title='TDS Sensor'></div><div id='gg5' class='col-4 gauge' data-value='" + String(value_devices_4) +"' data-title='Turbidity Sensor'></div><div id='gg6' class='col-4 gauge' data-value='" + String(value_devices_5) +"' data-title='Water Temperature'></div><div id='gg7' class='col-4 gauge' data-value='" + String(value_devices_6) +"' data-title='PH Water Sensor'></div></div></main>";
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

  String NewSendInterval = readEEPROM(0);
  String NewEndpointUrl = readEEPROM(NewSendInterval.length() + 1);
  String NewAuthSecretKey = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + 2);
  String NewDeviceApiKey  = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + 3);
  String NewDeviceApiKey_2 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + 4);
  String NewDeviceApiKey_3 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + 5);
  String NewDeviceApiKey_4 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + 6);
  String NewDeviceApiKey_5 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + 7);          
  String NewDeviceApiKey_6 = readEEPROM(NewSendInterval.length() + NewEndpointUrl.length() + NewAuthSecretKey.length() + NewDeviceApiKey.length() + NewDeviceApiKey_2.length() + NewDeviceApiKey_3.length() + NewDeviceApiKey_4.length() + NewDeviceApiKey_5.length() + 8);          
  
  String SendInterval = String(NewSendInterval);
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
  html += "<th>Configuration</th>";
  html += "<th>Value</th>";
  html += "</tr>";
  html += "</thead>";
  html += "<tbody>";
  html += "<tr>";
  html += "<th scope='row'>1</th>";
  html += "<td>Data Sending Interval (Minutes)</td>";
  html += "<td><input type='number' id='interval' name='interval' value='" + SendInterval + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>2</th>";
  html += "<td>Endpoint API Url</td>";
  html += "<td><input type='text' id='enpoint' name='serverName' value='" + EndpointUrl + "' style='width:100%;'></td>";
  html += "</tr>";
  html += "<tr>";
  html += "<th scope='row'>3</th>";
  html += "<td>Secret Key</td>";
  html += "<td><textarea class='form-control' id='token' name='secretKey' rows='3'>" + AuthSecretKey + "</textarea></td>";
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
  html += "<th>Update Value</th>";
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
      String SendInterval = server.arg("interval");
      String EndpointUrl = server.arg("serverName");
      String AuthSecretKey = server.arg("secretKey");
      String DeviceApiKey = server.arg("deviceKey");
      String DeviceApiKey_2 = server.arg("deviceKey_2");
      String DeviceApiKey_3 = server.arg("deviceKey_3");
      String DeviceApiKey_4 = server.arg("deviceKey_4");
      String DeviceApiKey_5 = server.arg("deviceKey_5");
      String DeviceApiKey_6 = server.arg("deviceKey_6");

      int addr = 0; // alamat awal penyimpanan di EEPROM
      saveEEPROM(addr, SendInterval);
      addr += SendInterval.length() + 1;
      saveEEPROM(addr, EndpointUrl);
      addr += EndpointUrl.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, AuthSecretKey);
      addr += AuthSecretKey.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, DeviceApiKey);
      addr += DeviceApiKey.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, DeviceApiKey_2);
      addr += DeviceApiKey_2.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, DeviceApiKey_3);
      addr += DeviceApiKey_3.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, DeviceApiKey_4);
      addr += DeviceApiKey_4.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, DeviceApiKey_5);
      addr += DeviceApiKey_5.length() + 1; // tambahkan panjang string + 1 untuk null terminator
      saveEEPROM(addr, DeviceApiKey_6);

      server.sendHeader("Location","/device-setup");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
}


void resetEEPROM() {
  for (size_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM resetted.");
}

void resetingEeprom(){
  resetEEPROM();
  wmresetsetting();
}

void setup_esp_server(){
  setupEEPROM();
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
}
void handle_esp_server(){
  server.handleClient();
}