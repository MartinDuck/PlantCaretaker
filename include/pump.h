#ifndef PUMP_H
#define PUMP_H
#include <Arduino.h>

const int PUMP_PIN = 1; 

extern bool isPumpActive;
extern unsigned long lastWateringTimestamp;
extern unsigned long blockUntillMillis;

void setupPump();
void startPump(int durationMs); 
int updatePump();             

#endif