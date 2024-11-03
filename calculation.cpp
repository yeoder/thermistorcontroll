#include "calculation.h"

const float R25 = 10000.0;
const float B_value = 3950.0;

float calculateTemperatureFromResistance(float resistance) {
    return (1.0 / ((1.0 / 298.15) + (1.0 / B_value) * log(resistance / R25))) - 273.15;
}