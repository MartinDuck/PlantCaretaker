#ifndef PUMP_H
#define PUMP_H
#include <Arduino.h>

const int PUMP_PIN = 1; 

void setupPump();
void startPump(int durationMs); 
void updatePump();             

#endif