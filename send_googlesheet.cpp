#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

// Enter Google Script Deployment ID:
const char *GScriptIdGo = "AKfycbx_HYGlKPJPb2u82cVqe2MvNBMrq_xjJLm8EIxY6Vw2IKiUlX7I2Jm3axRUsp3Ghb-z";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* hostGo = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptIdGo + "/exec";
HTTPSRedirect* clientgo = nullptr;

// Declare variables that will be published to Google Sheets
int value0 = 0;
int value1 = 0;
int value2 = 0;

void setup_googlesheet(){
  delay(10);
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  Serial.println('\n');

  // Use HTTPSRedirect class to create a new TLS connection
  clientgo = new HTTPSRedirect(httpsPort);
  clientgo->setInsecure();
  clientgo->setPrintResponseBody(true);
  clientgo->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(hostGo);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = clientgo->connect(hostGo, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(hostGo);
    return;
  }
  delete clientgo;    // delete HTTPSRedirect object
  clientgo = nullptr; // delete HTTPSRedirect object

}
void loop_googlesheet(){
    // create some fake data to publish
  value0 ++;
  value1 = random(0,1000);
  value2 = random(0,100000);


  static bool flag = false;
  if (!flag){
    clientgo = new HTTPSRedirect(httpsPort);
    clientgo->setInsecure();
    flag = true;
    clientgo->setPrintResponseBody(true);
    clientgo->setContentTypeHeader("application/json");
  }
  if (clientgo != nullptr){
    if (!clientgo->connected()){
      clientgo->connect(hostGo, httpsPort);
    }
  }
  else{
    Serial.println("Error creating clientgo object!");
  }
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + value0 + "," + value1 + "," + value2 + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(clientgo->POST(url, hostGo, payload)){ 
    // do stuff here if publish was successful
  }
  else{
    // do stuff here if publish was not successful
    Serial.println("Error while connecting");
  }

  // a delay of several seconds is required before publishing again    
  delay(5000);

}

