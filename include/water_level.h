#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <Arduino.h>

extern int tankEmptyDist;
extern int tankFullDist;
void setupWaterLevelSensor();
int readWaterLevelPercent(int minDistanceCm, int maxDistanceCm);
int getDistanceCm();
int saveTankSettings(int e, int f);
void loadTankSettings();    

#endif