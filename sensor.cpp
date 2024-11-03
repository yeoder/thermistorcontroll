// sensor.cpp
#include "sensor.h"
#include <math.h> // För log-funktionen

const float R_fixed = 10000.0; // Fixed resistor value
const float V_in = 5.0;        // Input voltage
const int sensorPin = 34;      // ADC pin for sensor input


// Definiera funktionen för att beräkna temperatur från sensorns motstånd
float calculateSensorTemperature(float R_thermistor) {
    float temperature = (1 / ((log(R_thermistor / R_fixed) / 3950) + (1 / 298.15))) - 273.15; // Celsius
    return temperature;
}

// Definition av readTemperature-funktionen
float readTemperature() {
    int sensorValue = analogRead(sensorPin);
    float V_out = sensorValue * (V_in / 4095.0);
    float R_thermistor = R_fixed * ((V_in / V_out) - 1);
    return calculateSensorTemperature(R_thermistor);
}

// Definition av calculateOhmFromTemperature-funktionen
float calculateOhmFromTemperature(float temperature, float R25, float B_value) {
    float T_kelvin = temperature + 273.15; // Konvertera Celsius till Kelvin
    return R25 * exp(B_value * ((1 / T_kelvin) - (1 / 298.15))); // Beräkna Ohm
}