#include "event_sender.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

void eventSenderSend(const String& ip,
                     const String& eventType,
                     const String& detail) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[EventSender] WiFi not connected, skipping");
        return;
    }

    StaticJsonDocument<256> doc;
    doc["attacker_ip"] = ip;
    doc["event_type"]  = eventType;
    doc["detail"]      = detail;
    doc["device_id"]   = DEVICE_ID;
    doc["uptime_ms"]   = (int)millis();

    String body;
    serializeJson(doc, body);

    HTTPClient http;
    http.begin(BACKEND_URL);
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(body);
    Serial.printf("[EventSender] POST -> HTTP %d\n", code);
    http.end();
}