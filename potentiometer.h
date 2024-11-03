#include <Arduino.h>

const int CS_PIN = 5;
const int UD_PIN = 15;
int currentResistance = 0;

void setupPotentiometer() {
    pinMode(CS_PIN, OUTPUT);
    pinMode(UD_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);  // Deselect the potentiometer
}

void setDigitalPotentiometer(int targetValue) {
    int steps = targetValue - currentResistance;
    digitalWrite(CS_PIN, LOW);  // Activate potentiometer (CS is LOW)

    digitalWrite(UD_PIN, steps > 0 ? HIGH : LOW);  // Set direction

    for (int i = 0; i < abs(steps); i++) {
        digitalWrite(CS_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(CS_PIN, LOW);
        delayMicroseconds(10);
    }

    digitalWrite(CS_PIN, HIGH);  // Deselect potentiometer
    currentResistance = targetValue;  // Store new resistance
}

int getCurrentResistance() {
    return currentResistance;
}