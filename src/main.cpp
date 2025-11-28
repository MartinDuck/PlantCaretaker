#include <Arduino.h>

// Definicja pinu do którego podłączona jest wbudowana dioda LED.
// Dla większości ESP32-C3 jest to GPIO 8.
const int ledPin = 8; 

void setup() {
  // Ustawienie pinu LED jako wyjście
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1. Włącz diodę (stan HIGH, czyli wysoki)
  digitalWrite(ledPin, HIGH); 
  
  // Poczekaj 1000 milisekund (1 sekunda)
  delay(1000); 
  
  // 2. Wyłącz diodę (stan LOW, czyli niski)
  digitalWrite(ledPin, LOW); 
  
  // Poczekaj 1000 milisekund (1 sekunda)
  delay(1000); 
}