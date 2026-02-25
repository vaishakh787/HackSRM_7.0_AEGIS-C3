#!/usr/bin/env bash
# =============================================================
#  AEGIS-C3 — Attack Simulator
#  Sends real HTTP requests directly to the XIAO ESP32-C3
#  honeypot webserver (port 80), triggering live detection:
#    XIAO detects → sends event to Flask → stored in InfluxDB
#                                        → shown in Grafana
#
#  Usage:
#    chmod +x scripts/simulate_attack.sh
#    ./scripts/simulate_attack.sh <XIAO-IP>
#
#  Example:
#    ./scripts/simulate_attack.sh 192.168.1.42
# =============================================================

set -euo pipefail

XIAO_IP="${1:-}"

if [ -z "$XIAO_IP" ]; then
    echo "Usage: $0 <XIAO-IP>"
    echo "  Example: $0 192.168.1.42"
    echo ""
    echo "Find the XIAO IP from the Serial monitor after boot:"
    echo "  [WiFi] Connected: 192.168.1.XX  ← that IP"
    exit 1
fi

HONEYPOT_URL="http://${XIAO_IP}"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

info()  { echo -e "${GREEN}[sim]${NC} $*"; }
warn()  { echo -e "${YELLOW}[sim]${NC} $*"; }
hit() {
    local method="$1" path="$2" extra="${3:-}"
    local url="${HONEYPOT_URL}${path}"
    local http_status

    if [ "$method" = "POST" ]; then
        http_status=$(curl -s -o /dev/null -w "%{http_code}" \
            -X POST "$url" \
            --data "$extra" \
            --connect-timeout 3 || echo "000")
    else
        http_status=$(curl -s -o /dev/null -w "%{http_code}" \
            -X GET "$url" \
            --connect-timeout 3 || echo "000")
    fi

    if [ "$http_status" = "000" ]; then
        echo -e "  ${RED}✗${NC} ${method} ${path} — no response (XIAO unreachable?)"
    else
        echo -e "  ${GREEN}✓${NC} ${CYAN}${method}${NC} ${path} — HTTP ${http_status}"
    fi
    sleep 0.5
}

# ── Reachability check ────────────────────────────────────────
echo ""
echo "============================================"
echo -e "  ${CYAN}AEGIS-C3 Attack Simulator${NC}"
echo "  Target: ${HONEYPOT_URL} (XIAO honeypot)"
echo "============================================"
echo ""
info "Checking XIAO reachability ..."
REACH=$(curl -s -o /dev/null -w "%{http_code}" "${HONEYPOT_URL}/admin" --connect-timeout 4 || echo "000")
if [ "$REACH" = "000" ]; then
    echo -e "${RED}[sim]${NC} Cannot reach XIAO at ${XIAO_IP}."
    echo "      - Is the XIAO powered and connected to WiFi?"
    echo "      - Is your laptop on the same network?"
    echo "      - Check Serial monitor for the correct IP."
    exit 1
fi
info "XIAO is reachable (HTTP ${REACH})."
echo ""

# ── Phase 1: Path scan (triggers SCAN detection) ─────────────
echo -e "${YELLOW}--- Phase 1: Path Scan (triggers scan detection on XIAO) ---${NC}"
echo "    Sending 6 GET requests to different paths ..."
echo "    SCAN_THRESHOLD=5 hits in 10s → detection fires"
echo ""
hit GET /admin
hit GET /login
hit GET /wp-admin
hit GET /phpmyadmin
hit GET /.env
hit GET /config.php

echo ""
echo "    Watch for on XIAO Serial:"
echo "    [Detection] SCAN detected from <your-laptop-IP>"
echo "    [EventSender] POST -> HTTP 200"
echo "    And: OLED shows !! ALERT !! + LED turns on"
echo ""
info "Waiting 8 seconds for XIAO alert to clear before Phase 2 ..."
sleep 8

# ── Phase 2: Brute-force (triggers BRUTE_FORCE detection) ────
echo -e "${YELLOW}--- Phase 2: Brute Force (triggers brute_force detection on XIAO) ---${NC}"
echo "    Sending 4 POST /admin requests with fake credentials ..."
echo "    BRUTE_THRESHOLD=3 POSTs in 30s → detection fires"
echo ""
hit POST /admin "user=admin&pass=password123"
hit POST /admin "user=admin&pass=admin"
hit POST /admin "user=root&pass=toor"
hit POST /admin "user=administrator&pass=letmein"

echo ""
echo "    Watch for on XIAO Serial:"
echo "    [Detection] BRUTE-FORCE detected from <your-laptop-IP>"
echo "    [EventSender] POST -> HTTP 200"
echo "    And: OLED shows !! ALERT !! + LED turns on"
echo ""

# ── Done ──────────────────────────────────────────────────────
echo "============================================"
echo -e "  ${GREEN}Simulation complete.${NC}"
echo ""
echo "  Expected end-to-end flow:"
echo "  XIAO detected hits → sent events to Flask backend"
echo "  → stored in InfluxDB → visible in Grafana"
echo ""
echo "  Open Grafana: http://localhost:3000"
echo "  Dashboard: AEGIS-C3 Attack Monitor"
echo "============================================"
