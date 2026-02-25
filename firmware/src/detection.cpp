#include "detection.h"
#include "alerts.h"
#include "event_sender.h"
#include "config.h"
#include <map>

struct IPRecord {
    int  requestCount = 0;
    int  postCount    = 0;
    unsigned long windowStart = 0;
};

static std::map<String, IPRecord> ipTracker;

void detectionRegisterHit(String ip, String detail) {
    unsigned long now = millis();
    IPRecord& rec = ipTracker[ip];

    // Reset window if expired (use longest window)
    if ((now - rec.windowStart) > BRUTE_WINDOW_MS) {
        rec.requestCount = 0;
        rec.postCount    = 0;
        rec.windowStart  = now;
    }

    rec.requestCount++;
    if (detail.startsWith("POST")) rec.postCount++;

    Serial.printf("[Detection] IP=%s hits=%d posts=%d detail=%s\n",
        ip.c_str(), rec.requestCount, rec.postCount, detail.c_str());

    // --- Brute-force rule ---
    if (rec.postCount >= BRUTE_THRESHOLD) {
        rec.postCount = 0;  // reset to avoid repeated firing
        alertsTrigger(ip, "brute_force");
        eventSenderSend(ip, "brute_force", detail);
        return;
    }

    // --- Scan rule ---
    if (rec.requestCount >= SCAN_THRESHOLD) {
        rec.requestCount = 0;  // reset to avoid repeated firing
        alertsTrigger(ip, "scan");
        eventSenderSend(ip, "scan", detail);
    }
}