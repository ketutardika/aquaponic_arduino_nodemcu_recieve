#include <Arduino.h>
#include <EEPROM.h>
#include "helper_function.h"

void powerOnLED(byte pin){
  digitalWrite(pin, HIGH);
}

void powerOffLED(byte pin){
  digitalWrite(pin, LOW);
}

void setupLED(byte pin){
  pinMode(pin, OUTPUT);
}

void setupEEPROM(){
  EEPROM.begin(4096);
}

void saveEEPROM(int addr, String data) {
  int len = data.length();
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + len, '\0'); // tambahkan null terminator
  EEPROM.commit();
}

String readEEPROM(int addr) {
  String data = "";
  char ch = EEPROM.read(addr);
  while (ch != '\0') {
    data += ch;
    addr++;
    ch = EEPROM.read(addr);
  }
  return data;
}