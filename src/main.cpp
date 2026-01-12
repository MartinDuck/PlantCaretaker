#include <Arduino.h>
#include <WiFiManager.h>     
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include "moisture_sensor.h"
#include "web_handlers.h"
#include "pump.h"

extern bool isPumpActive;
AsyncWebServer server(8080);

void setup() {
    delay(1000); 
    Serial.begin(115200);

    setupSensor();
    setupPump();
    analogReadResolution(12); 

    WiFiManager wm;

    wm.setConfigPortalTimeout(180);
    bool res = wm.autoConnect("SmartPlant_Config");

    if(!res) {
        Serial.println("Błąd połączenia!");
        ESP.restart();
    }
    
    if (WiFi.getMode() & WIFI_AP) {
        Serial.println("Konfiguracja zakończona sukcesem. Restartuję...");
        delay(2000);
        ESP.restart();
    }

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    Serial.println("WiFi OK! Startuję serwer...");
    delay(1000);

    if (!MDNS.begin("caretaker")) {
        Serial.println("Błąd konfiguracji mDNS!");
    } else {
        Serial.println("mDNS uruchomiony: http://caretaker.local");
        MDNS.addService("http", "tcp", 8080);
    }

    if(!LittleFS.begin(true)){
        Serial.println("Blad LittleFS!");
    }
    
    setupWebRoutes(&server);
    server.begin();
    Serial.println("Serwer uruchomiony.");



}

int moistureLevel = 0;
unsigned long previousMillis = 0;
const long interval = 5000;

void loop() {
   updatePump();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (!isPumpActive) {
        
        moistureLevel = readPercent(4095, 1500); 
        Serial.print("Poziom wilgotności gleby: ");
        Serial.print(moistureLevel);
        Serial.println("%");
        
        }
    }
}