#pragma once

// --- Wi-Fi ---
#define WIFI_SSID     "YOUR_SSID"
#define WIFI_PASS     "YOUR_PASSWORD"

// --- Backend ---
#define BACKEND_URL   "http://192.168.1.X:5000/event"  // Replace X
#define DEVICE_ID     "aegis-c3-node-01"

// --- Detection thresholds ---
#define SCAN_THRESHOLD    5
#define BRUTE_THRESHOLD   3
#define SCAN_WINDOW_MS    10000
#define BRUTE_WINDOW_MS   30000

// --- Hardware pins ---
// ASSUMPTION: ESP32-C3 DevKitM-1 default I2C pins
#define PIN_SDA       8
#define PIN_SCL       9
#define PIN_LED       4