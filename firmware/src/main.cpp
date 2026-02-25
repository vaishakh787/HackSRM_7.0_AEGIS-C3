// =============================================================
//  AEGIS-C3 — Main Entry Point
//  Hardware: Seeed Studio XIAO ESP32-C3
// =============================================================

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "honeypot.h"
#include "alerts.h"

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("[WiFi] Connecting");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        attempts++;
        if (attempts > 40) {
            // After 20 seconds, restart and retry
            Serial.println("\n[WiFi] Timeout — restarting...");
            ESP.restart();
        }
    }
    Serial.println("\n[WiFi] Connected: " + WiFi.localIP().toString());
    Serial.println("[WiFi] RSSI: " + String(WiFi.RSSI()) + " dBm");
}

void setup() {
    // XIAO ESP32-C3: Serial over hardware UART, no delay needed
    Serial.begin(115200);
    delay(500);

    Serial.println("\n=============================");
    Serial.println("       AEGIS-C3 BOOT         ");
    Serial.println("=============================");

    alertsInit();           // OLED + LED init first — visual boot feedback
    alertsShowStatus("Booting...");

    connectWiFi();
    alertsShowStatus("WiFi OK");

    honeypotInit();         // Start AsyncWebServer on port 80
    alertsShowStatus("Monitoring...");

    Serial.println("[AEGIS-C3] System ready");
    Serial.println("[AEGIS-C3] Honeypot: http://" + WiFi.localIP().toString() + "/admin");
    Serial.println("[AEGIS-C3] Backend:  " + String(BACKEND_URL));
}

void loop() {
    // AsyncWebServer is event-driven — loop is intentionally free
    // Add non-blocking heartbeat here if needed in future
    delay(100);
}