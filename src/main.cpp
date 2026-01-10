#include <Arduino.h>
#include <WiFiManager.h>     
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include "sensor.h"
#include "web_handlers.h"

AsyncWebServer server(80);


void setup() {
    delay(1000); 
    Serial.begin(115200);

    setupSensor();
    analogReadResolution(12); 

    WiFiManager wm;
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

    Serial.println("WiFi OK! Startuję serwer...");

    if (!MDNS.begin("caretaker")) {
        Serial.println("Błąd konfiguracji mDNS!");
    } else {
        Serial.println("mDNS uruchomiony: http://caretaker.local");
        MDNS.addService("http", "tcp", 80);
    }

    if(!LittleFS.begin(true)){
        Serial.println("Blad LittleFS!");
    }

    setupWebRoutes(&server);
    server.begin();
    Serial.println("Serwer uruchomiony.");
}

void loop() {
   
}