#!/usr/bin/env bash
# =============================================================
#  AEGIS-C3 — Attack Simulator
#  Sends fake honeypot events directly to the Flask backend
#  to test the full pipeline (Flask → InfluxDB → Grafana)
#  without needing the ESP32 hardware.
#
#  Usage:
#    chmod +x scripts/simulate_attack.sh
#    ./scripts/simulate_attack.sh [backend_url]
#
#  Examples:
#    ./scripts/simulate_attack.sh
#    ./scripts/simulate_attack.sh http://192.168.1.42:5000
# =============================================================

set -euo pipefail

BACKEND_URL="${1:-http://localhost:5000}"
ENDPOINT="${BACKEND_URL}/event"
DEVICE_ID="aegis-c3-sim-01"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

info()    { echo -e "${GREEN}[sim]${NC} $*"; }
warn()    { echo -e "${YELLOW}[sim]${NC} $*"; }
send_event() {
    local ip="$1" type="$2" detail="$3"
    local payload
    payload=$(printf '{"attacker_ip":"%s","event_type":"%s","detail":"%s","device_id":"%s","uptime_ms":%d}' \
        "$ip" "$type" "$detail" "$DEVICE_ID" "$((RANDOM * 10))")

    HTTP_STATUS=$(curl -s -o /tmp/aegis_sim_resp.txt -w "%{http_code}" \
        -X POST "${ENDPOINT}" \
        -H "Content-Type: application/json" \
        -d "$payload")

    if [ "$HTTP_STATUS" -eq 200 ]; then
        echo -e "  ${GREEN}✓${NC} ${CYAN}${type}${NC} from ${ip} — ${detail}"
    else
        echo -e "  ${RED}✗${NC} HTTP ${HTTP_STATUS} — $(cat /tmp/aegis_sim_resp.txt)"
    fi
}

# ── Health check first ────────────────────────────────────────
echo ""
echo "============================================"
echo -e "  ${CYAN}AEGIS-C3 Attack Simulator${NC}"
echo "  Backend: ${ENDPOINT}"
echo "============================================"

HTTP_HEALTH=$(curl -s -o /dev/null -w "%{http_code}" "${BACKEND_URL}/health" || true)
if [ "$HTTP_HEALTH" -ne 200 ]; then
    echo -e "${RED}[sim]${NC} Backend not reachable at ${BACKEND_URL} (HTTP ${HTTP_HEALTH})."
    echo "       Start the backend first:  cd backend && python app.py"
    exit 1
fi
info "Backend is healthy."
echo ""

# ── Scenario 1: Port/path scan ────────────────────────────────
echo -e "${YELLOW}--- Scenario 1: Path Scan (5 hits from 10.0.0.1) ---${NC}"
for path in /admin /login /wp-admin /phpmyadmin /.env; do
    send_event "10.0.0.1" "scan" "SCAN ${path}"
    sleep 0.3
done

echo ""

# ── Scenario 2: Brute-force login ─────────────────────────────
echo -e "${YELLOW}--- Scenario 2: Brute Force (3 POST attempts from 10.0.0.2) ---${NC}"
for creds in "admin:password" "admin:admin123" "root:toor"; do
    user="${creds%%:*}"
    pass="${creds##*:}"
    send_event "10.0.0.2" "brute_force" "POST /admin user=${user}"
    sleep 0.3
done

echo ""

# ── Scenario 3: Mixed traffic from multiple IPs ───────────────
echo -e "${YELLOW}--- Scenario 3: Multi-IP Mixed Traffic ---${NC}"
send_event "172.16.0.5"  "scan"        "SCAN /config.php"
send_event "172.16.0.5"  "scan"        "SCAN /backup.zip"
send_event "192.168.50.9" "brute_force" "POST /admin user=administrator"
send_event "192.168.50.9" "brute_force" "POST /admin user=admin"
send_event "10.10.10.10"  "scan"        "SCAN /.git/config"

echo ""
echo "============================================"
echo -e "  ${GREEN}Simulation complete.${NC}"
echo "  Open Grafana at http://localhost:3000"
echo "  Dashboard: AEGIS-C3 Attack Monitor"
echo "============================================"
