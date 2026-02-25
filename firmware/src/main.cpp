#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "honeypot.h"
#include "alerts.h"

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("[WiFi] Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Connected: " + WiFi.localIP().toString());
}

void setup() {
    Serial.begin(115200);
    delay(1000);  // Required on some ESP32-C3 boards for USB CDC

    alertsInit();      // OLED + LED init first for visual feedback
    connectWiFi();
    honeypotInit();    // Start web server

    alertsShowStatus("Monitoring...");
    Serial.println("[AEGIS-C3] Ready");
}

void loop() {
    // AsyncWebServer is interrupt-driven; loop stays free
    // Add watchdog or heartbeat here if needed
    delay(100);
}