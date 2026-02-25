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
//  LED pin:
//    GPIO 3 (D1) — safe general-purpose output
//
//  STRAPPING PINS — DO NOT use for external hardware:
//    GPIO 2 (D0) — strapping pin, affects boot mode if pulled LOW at power-on
//    GPIO 8 (D8) — strapping pin, must be HIGH during download boot
//    GPIO 9 (D9) — strapping pin, connected to onboard BOOT button
//
//  OTHER RESERVED PINS:
//    GPIO 18/19 — used internally for USB-JTAG on XIAO, do not use
//    GPIO 20/21 — UART0 RX/TX (Serial monitor), avoid for other use
//    GPIO 12-17 — reserved for internal SPI flash, do not use
//
//  ADC NOTE:
//    A3 (GPIO 5 / D3) uses ADC2, which can produce false readings.
//    Use A0/A1/A2 (GPIO 2/3/4, ADC1) for reliable analog reads.
// =============================================================

#define PIN_SDA   6   // D4 on XIAO silkscreen — I2C SDA
#define PIN_SCL   7   // D5 on XIAO silkscreen — I2C SCL
#define PIN_LED   3   // D1 on XIAO silkscreen — connect LED + 220Ω to GND
                      // NOTE: was GPIO2 (D0) — moved off strapping pin per official docs