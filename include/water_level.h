#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <Arduino.h>

void setupWaterLevelSensor();
int readWaterLevelPercent();
int getDistnaceCm();

#endif