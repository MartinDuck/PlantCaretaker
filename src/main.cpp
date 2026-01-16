#include <Arduino.h>
#include <WiFiManager.h>     
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include "moisture_sensor.h"
#include "web_handlers.h"
#include "pump.h"
#include "water_level.h"
#include <Preferences.h>

AsyncWebServer server(8080);

void setup() {
    delay(1000); 
    Serial.begin(115200);

    setupSensor();
    setupPump();
    setupWaterLevelSensor();
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

    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.google.com");

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
unsigned long previousMoisture = 0;
unsigned long previousWaterLevel = 0;
const long moisture_interval = 1200000; // 20 minutes
const long water_level_interval = 3600000; // 1 hour


int waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);


void loop() {
   updatePump();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMoisture >= moisture_interval) {
        previousMoisture = currentMillis;

        if (!isPumpActive && blockUntillMillis < millis()) {
        
        moistureLevel = readPercent(4095, 1500); 
        Serial.print("Poziom wilgotności gleby: ");
        Serial.print(moistureLevel);
        Serial.println("%");
        }
    }

    if (currentMillis - previousWaterLevel >= water_level_interval) {
        previousWaterLevel = currentMillis;
        waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);
        Serial.print("Poziom wody w zbiorniku: ");
        Serial.print(waterLevelPercent);
        Serial.println("%");
    }


}