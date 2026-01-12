#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

const int MOISTURE_SENSOR_SIGNAL = 2; 
const int MOISTURE_SENSOR_POWER = 3;

void setupSensor();

int readRaw();

int readPercent(int dryValue, int wetValue);

#endif
