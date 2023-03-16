#ifndef HELPER_FUNCTION_H
#define HELPER_FUNCTION_H
#include <Arduino.h>

void powerOnLED(byte pin);
void powerOffLED(byte pin);
void setupLED(byte pin);
void setupEEPROM();
void saveEEPROM(int addr, String data);
String readEEPROM(int addr);

#endif