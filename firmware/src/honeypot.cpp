#include "honeypot.h"
#include "detection.h"
#include <ESPAsyncWebServer.h>

static AsyncWebServer server(80);

static const char LOGIN_HTML[] PROGMEM = R"(
<!DOCTYPE html><html><body>
<h2>Admin Panel</h2>
<form method='POST' action='/admin'>
  Username: <input name='user'><br>
  Password: <input type='password' name='pass'><br>
  <input type='submit' value='Login'>
</form></body></html>
)";

void honeypotInit() {
    // GET /admin — serve fake login page
    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest* req) {
        String ip = req->client()->remoteIP().toString();
        detectionRegisterHit(ip, "GET /admin");
        req->send_P(200, "text/html", LOGIN_HTML);
    });

    // POST /admin — capture credentials
    server.on("/admin", HTTP_POST, [](AsyncWebServerRequest* req) {
        String ip   = req->client()->remoteIP().toString();
        String user = req->hasParam("user", true) ? req->getParam("user", true)->value() : "";
        String pass = req->hasParam("pass", true) ? req->getParam("pass", true)->value() : "";
        detectionRegisterHit(ip, "POST /admin user=" + user + " pass=" + pass);
        req->send(401, "text/plain", "Unauthorized");
    });

    // Catch-all — log path scanners
    server.onNotFound([](AsyncWebServerRequest* req) {
        String ip   = req->client()->remoteIP().toString();
        String path = req->url();
        detectionRegisterHit(ip, "SCAN " + path);
        req->send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("[Honeypot] Listening on port 80");
}