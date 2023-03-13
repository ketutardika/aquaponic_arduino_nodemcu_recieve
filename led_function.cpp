#include "led_function.h"

void powerOnLED(byte pin){
  digitalWrite(pin, HIGH);
}

void powerOffLED(byte pin){
  digitalWrite(pin, LOW);
}

void setupLED(byte pin){
  pinMode(pin, OUTPUT);
}