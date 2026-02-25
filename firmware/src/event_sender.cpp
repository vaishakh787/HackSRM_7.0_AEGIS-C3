#include "alerts.h"
#include "config.h"
#include <U8g2lib.h>
#include <Wire.h>

// ASSUMPTION: SH1106 controller (common for 1.3" OLEDs)
// If display is blank, swap to: U8G2_SSD1306_128X64_NONAME_F_HW_I2C
static U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

static void oledPrint(const char* line1,
                      const char* line2 = "",
                      const char* line3 = "") {
    display.clearBuffer();
    display.setFont(u8g2_font_6x10_tf);
    display.drawStr(0, 12, line1);
    display.drawStr(0, 28, line2);
    display.drawStr(0, 44, line3);
    display.sendBuffer();
}

void alertsInit() {
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    Wire.begin(PIN_SDA, PIN_SCL);
    display.begin();
    oledPrint("AEGIS-C3", "Booting...");
}

void alertsShowStatus(const String& msg) {
    oledPrint("AEGIS-C3", msg.c_str());
}

void alertsTrigger(const String& ip, const String& eventType) {
    Serial.printf("[Alert] %s from %s\n", eventType.c_str(), ip.c_str());

    digitalWrite(PIN_LED, HIGH);
    oledPrint("!! ALERT !!",
              ("Type: " + eventType).c_str(),
              ("IP: "   + ip).c_str());

    delay(5000);  // NOTE: blocks — acceptable for MVP demo

    digitalWrite(PIN_LED, LOW);
    oledPrint("AEGIS-C3", "Monitoring...", ("Last: " + ip).c_str());
}