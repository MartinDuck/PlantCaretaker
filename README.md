# SmartPlant Caretaker (ESP32-C3)

## Project Overview

The SmartPlant Caretaker is an autonomous, IoT-enabled irrigation system designed for domestic plant cultivation. It utilizes the ESP32-C3-SuperMini microcontroller to monitor soil moisture and water reservoir levels, providing automated water delivery through a proportional dosing algorithm. The system features a web-based dashboard for real-time telemetry and remote configuration.

## Hardware Components

* **Microcontroller:** ESP32-C3-SuperMini (RISC-V architecture)
* **Soil Sensor:** Gold-plated resistive moisture probe
* **Level Sensor:** HC-SR04 ultrasonic transducer
* **Actuator:** DC submersible water pump
* **Isolation:** 5V Relay module
* **Power Supply:** 5V USB-C interface (standard mobile charger)

## Core Features

* **Proportional Dosing:** The system calculates pump run-time based on the moisture deficit, preventing over-saturation.
* **Soaking Period:** Implements a 15-minute lockout (dead time) after irrigation to allow for water diffusion through the substrate.
* **Safety Lockout:** Automatic pump inhibition if the reservoir water level falls below the user-defined minimum threshold.
* **Persistent Storage:** All user settings, including calibration data and moisture thresholds, are saved in Non-Volatile Storage (NVS).
* **Responsive Web UI:** A Single-Page Application (SPA) served via LittleFS for local network monitoring and control.

## Software Architecture

The firmware is developed in C++ using the Arduino Framework. It utilizes an asynchronous design to ensure the web server remains responsive regardless of sensor sampling or irrigation cycles.

* **ESPAsyncWebServer:** Handles HTTP requests and serves the static dashboard files.
* **WiFiManager:** Manages network connectivity through a captive portal.
* **Preferences:** Manages non-volatile data storage.
* **Fetch API:** Used by the frontend for non-blocking data updates.

## Installation and Configuration

1. **Hardware Assembly:** Connect sensors and actuators according to the provided pinout schematics.
2. **Filesystem Upload:** Upload the contents of the `data` folder to the ESP32 using the LittleFS Data Upload tool.
3. **Firmware Flash:** Compile and upload the C++ code to the ESP32-C3.
4. **Initial Setup:**
* Connect to the `SmartPlant_Config` Access Point.
* Access the local dashboard via the provided IP or hostname.
* Calibrate the tank dimensions (Empty and Full distances) in the settings menu.
* Set the target moisture threshold and enable Automatic Mode.



## System Logic Flow

The system follows a specific sequence to ensure plant safety and hardware longevity:

1. **Sample:** Read soil conductivity and ultrasonic distance.
2. **Evaluate:** Check if current moisture is below the target threshold.
3. **Safety Check:** Verify water level is above the minimum required percentage.
4. **Actuate:** Run the pump for a duration proportional to the moisture deficit.
5. **Cooldown:** Enter a "Soaking State" for 15 minutes to allow for hydraulic equilibrium in the soil.

## System Limitations

* **Local Access:** The control interface is currently restricted to the local area network for security and latency reasons.
* **Point-Source Irrigation:** Best suited for small to medium pots where capillary action can distribute moisture effectively from a single outlet.



---
