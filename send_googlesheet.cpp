#include <Arduino.h>
#include <WiFiClientSecure.h>

const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure clientgo;
const char * request = "GET /macros/s/AKfycbzn4nRyhkLOwbO9rOMtFONaK2MYrDHMr8u1ts3_PuBFoaI8E4--YByGKeUaWaIEXydcog/exec?value1=42&value2=45&value3=40 HTTP/1.1";


void setup_googlesheet(){

};

void loop_googlesheet(){
  clientgo.setInsecure();
  if (!clientgo.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  clientgo.println(request);
  clientgo.print("Host: "); clientgo.println(host);
  clientgo.println("content-type: text/html");
  clientgo.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36");
  clientgo.println("Connection: close\r\n");

  Serial.println("request sent");
  clientgo.stop();
  delay(20000);
};
