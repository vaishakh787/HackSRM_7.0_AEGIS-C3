// =============================================================
//  AEGIS-C3 — Honeypot Service
//  Serves a fake admin login page and logs all requests
// =============================================================

#include "honeypot.h"
#include "detection.h"
#include <ESPAsyncWebServer.h>

static AsyncWebServer server(80);

// Fake admin login page — convincing enough for a demo
static const char LOGIN_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Admin Login</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #1a1a2e;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
    }
    .box {
      background: #16213e;
      padding: 40px;
      border-radius: 8px;
      border: 1px solid #0f3460;
      width: 300px;
    }
    h2 { color: #e94560; text-align: center; margin-bottom: 24px; }
    label { color: #a8a8b3; font-size: 13px; }
    input[type=text], input[type=password] {
      width: 100%;
      padding: 10px;
      margin: 6px 0 16px 0;
      background: #0f3460;
      border: 1px solid #e94560;
      border-radius: 4px;
      color: white;
      box-sizing: border-box;
    }
    input[type=submit] {
      width: 100%;
      padding: 12px;
      background: #e94560;
      border: none;
      border-radius: 4px;
      color: white;
      font-size: 15px;
      cursor: pointer;
    }
    .footer { color: #555; text-align: center; font-size: 11px; margin-top: 16px; }
  </style>
</head>
<body>
  <div class="box">
    <h2>&#128274; Admin Panel</h2>
    <form method='POST' action='/admin'>
      <label>Username</label>
      <input type='text' name='user' placeholder='admin' required>
      <label>Password</label>
      <input type='password' name='pass' placeholder='password' required>
      <input type='submit' value='Login'>
    </form>
    <div class="footer">Network Management System v2.1</div>
  </div>
</body>
</html>
)rawhtml";

// 401 response after credential capture
static const char DENIED_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html><html><body style="font-family:Arial;background:#1a1a2e;color:#e94560;
text-align:center;padding-top:80px;">
<h2>Access Denied</h2><p style="color:#a8a8b3;">Invalid credentials. This attempt has been logged.</p>
</body></html>
)rawhtml";

void honeypotInit() {
    // GET /admin — serve fake login page, count as a hit
    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest* req) {
        String ip = req->client()->remoteIP().toString();
        detectionRegisterHit(ip, "GET /admin");
        req->send_P(200, "text/html", LOGIN_HTML);
    });

    // POST /admin — capture submitted credentials, always deny
    server.on("/admin", HTTP_POST, [](AsyncWebServerRequest* req) {
        String ip   = req->client()->remoteIP().toString();
        String user = req->hasParam("user", true)
                      ? req->getParam("user", true)->value() : "(empty)";
        String pass = req->hasParam("pass", true)
                      ? req->getParam("pass", true)->value() : "(empty)";

        Serial.printf("[Honeypot] Credential attempt — IP=%s user=%s pass=%s\n",
                      ip.c_str(), user.c_str(), pass.c_str());

        detectionRegisterHit(ip, "POST /admin user=" + user);
        req->send_P(401, "text/html", DENIED_HTML);
    });

    // Catch-all — any other path is a scanner probing for endpoints
    server.onNotFound([](AsyncWebServerRequest* req) {
        String ip   = req->client()->remoteIP().toString();
        String path = req->url();
        detectionRegisterHit(ip, "SCAN " + path);
        req->send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("[Honeypot] Listening on port 80");
}