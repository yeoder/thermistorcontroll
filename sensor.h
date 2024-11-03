// sensor.h
#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

float calculateTemperature(float R_thermistor); // Deklaration av funktionen
float calculateOhmFromTemperature(float temperature, float R25, float B_value); // Deklaration för att beräkna Ohm
float readTemperature(); // Deklaration av funktionen

#endif // SENSOR_H
