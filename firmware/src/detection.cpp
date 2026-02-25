// =============================================================
//  AEGIS-C3 — Detection Engine
//  Rule-based scan and brute-force detection using per-IP counters
// =============================================================

#include "detection.h"
#include "alerts.h"
#include "event_sender.h"
#include "config.h"
#include <map>

// Per-IP tracking record
struct IPRecord {
    int           requestCount = 0;  // total requests in window
    int           postCount    = 0;  // POST /admin attempts in window
    unsigned long windowStart  = 0;  // millis() when window opened
};

// In-memory IP tracker
// ASSUMPTION: safe for demo with <20 unique IPs; no heap issues on XIAO ESP32-C3 (400KB SRAM)
static std::map<String, IPRecord> ipTracker;

void detectionRegisterHit(String ip, String detail) {
    unsigned long now = millis();
    IPRecord& rec = ipTracker[ip];

    // Reset window if the longest window (brute) has expired
    if ((now - rec.windowStart) > BRUTE_WINDOW_MS) {
        rec.requestCount = 0;
        rec.postCount    = 0;
        rec.windowStart  = now;
    }

    rec.requestCount++;
    if (detail.startsWith("POST")) {
        rec.postCount++;
    }

    Serial.printf("[Detection] IP=%-15s  hits=%d  posts=%d  detail=%s\n",
        ip.c_str(), rec.requestCount, rec.postCount, detail.c_str());

    // --- Rule 1: Brute-force (checked first — higher priority) ---
    if (rec.postCount >= BRUTE_THRESHOLD) {
        rec.postCount = 0;  // reset counter to avoid re-firing every hit
        Serial.printf("[Detection] BRUTE-FORCE detected from %s\n", ip.c_str());
        // FIX: send event BEFORE alertsTrigger to avoid 5s delay blocking HTTP POST
        eventSenderSend(ip, "brute_force", detail);
        alertsTrigger(ip, "brute_force");
        return;
    }

    // --- Rule 2: Port/path scan ---
    if (rec.requestCount >= SCAN_THRESHOLD) {
        rec.requestCount = 0;  // reset counter to avoid re-firing every hit
        Serial.printf("[Detection] SCAN detected from %s\n", ip.c_str());
        // FIX: send event BEFORE alertsTrigger to avoid 5s delay blocking HTTP POST
        eventSenderSend(ip, "scan", detail);
        alertsTrigger(ip, "scan");
    }
}