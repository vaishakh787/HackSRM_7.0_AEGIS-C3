#pragma once

// =============================================================
//  AEGIS-C3 — Configuration
//  Hardware: Seeed Studio XIAO ESP32-C3
// =============================================================

// --- Wi-Fi ---
#define WIFI_SSID     "YOUR_SSID"
#define WIFI_PASS     "YOUR_PASSWORD"

// --- Backend ---
#define BACKEND_URL   "http://192.168.1.X:5000/event"  // Replace X with your laptop IP
#define DEVICE_ID     "aegis-c3-node-01"

// --- Detection thresholds ---
#define SCAN_THRESHOLD    5      // unique path hits within SCAN_WINDOW_MS
#define BRUTE_THRESHOLD   3      // POST attempts within BRUTE_WINDOW_MS
#define SCAN_WINDOW_MS    10000  // 10 seconds
#define BRUTE_WINDOW_MS   30000  // 30 seconds

// =============================================================
//  XIAO ESP32-C3 Pin Reference
//  https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
//
//  I2C default pins on XIAO ESP32-C3:
//    SDA -> GPIO 6 (D4)
//    SCL -> GPIO 7 (D5)
//
//  Available GPIO for LED:
//    GPIO 2 (D0) — safe general-purpose output
//
//  NOTE: GPIO 18/19 are used internally for USB on XIAO.
//        GPIO 20/21 are used for UART0 (Serial).
//        Avoid those pins.
// =============================================================

#define PIN_SDA   6   // D4 on XIAO silkscreen
#define PIN_SCL   7   // D5 on XIAO silkscreen
#define PIN_LED   2   // D0 on XIAO silkscreen — connect LED + 220Ω to GND