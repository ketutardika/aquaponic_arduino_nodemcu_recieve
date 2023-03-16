#define READ_SERIAL_H
#include <Arduino.h>

void setup_read_serial();
void readSensor();
float readTemperature();
float readHumidity();
float readTds();
float readTurbidity();
float readWaterTemp();
float readPh();
void readSerialData();
void sendIP();