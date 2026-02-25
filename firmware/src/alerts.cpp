// =============================================================
//  AEGIS-C3 — Alerts Module
//  Controls OLED display and GPIO LED
//  Hardware: Seeed Studio XIAO ESP32-C3
//
//  OLED wiring:
//    SDA -> GPIO 6 (D4)
//    SCL -> GPIO 7 (D5)
//    VCC -> 3.3V
//    GND -> GND
//
//  LED wiring:
//    Anode -> GPIO 2 (D0) via 220Ω resistor -> GND
//
//  ASSUMPTION: 1.3" OLED uses SH1106 controller.
//  If display is blank or garbled, swap the U8G2 constructor below
//  to: U8G2_SSD1306_128X64_NONAME_F_HW_I2C
// =============================================================

#include "alerts.h"
#include "config.h"
#include <U8g2lib.h>
#include <Wire.h>

// SH1106 — common controller for 1.3" OLEDs
// HW_I2C uses the hardware I2C peripheral (faster, more reliable than SW)
static U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// ---------------------------------------------------------------
//  Internal helper: render up to 3 lines on the OLED
// ---------------------------------------------------------------
static void oledDraw(const char* line1,
                     const char* line2 = "",
                     const char* line3 = "") {
    display.clearBuffer();
    display.setFont(u8g2_font_6x10_tf);

    // Header bar
    display.drawBox(0, 0, 128, 14);
    display.setDrawColor(0);
    display.drawStr(2, 11, line1);
    display.setDrawColor(1);

    // Body lines
    display.drawStr(2, 28, line2);
    display.drawStr(2, 44, line3);

    // Bottom separator
    display.drawHLine(0, 54, 128);
    display.setFont(u8g2_font_5x7_tf);
    display.drawStr(2, 63, "AEGIS-C3 v1.0");

    display.sendBuffer();
}

// ---------------------------------------------------------------
//  Public API
// ---------------------------------------------------------------

void alertsInit() {
    // LED
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);

    // XIAO ESP32-C3: explicitly set I2C pins before display.begin()
    Wire.begin(PIN_SDA, PIN_SCL);

    // OLED
    display.begin();
    oledDraw("AEGIS-C3", "Booting...", "");
    Serial.println("[Alerts] OLED + LED initialised");
    Serial.printf("[Alerts] SDA=GPIO%d  SCL=GPIO%d  LED=GPIO%d\n",
                  PIN_SDA, PIN_SCL, PIN_LED);
}

void alertsShowStatus(const String& msg) {
    oledDraw("AEGIS-C3", msg.c_str(), "Monitoring...");
}

void alertsTrigger(const String& ip, const String& eventType) {
    Serial.printf("[Alert] TRIGGERED — type=%s ip=%s\n",
                  eventType.c_str(), ip.c_str());

    // LED on
    digitalWrite(PIN_LED, HIGH);

    // OLED alert screen
    String typeLabel = (eventType == "brute_force") ? "BRUTE FORCE" : "SCAN";
    oledDraw("!! ALERT !!",
             ("Type: " + typeLabel).c_str(),
             ("IP:   " + ip).c_str());

    // Hold alert visible for 5 seconds
    // NOTE: this blocks AsyncWebServer request handling during this window
    // Acceptable for MVP demo — requests during window are queued, not lost
    delay(5000);

    // LED off — return to monitoring display
    digitalWrite(PIN_LED, LOW);
    oledDraw("AEGIS-C3", "Monitoring...", ("Last: " + ip).c_str());
}