#!/usr/bin/env bash
# =============================================================
#  AEGIS-C3 — InfluxDB Setup Script
#  Run this AFTER `docker compose up -d` to verify that
#  InfluxDB initialised correctly and the bucket is ready.
#
#  Usage:
#    chmod +x scripts/setup_influxdb.sh
#    ./scripts/setup_influxdb.sh
# =============================================================

set -euo pipefail

# ---------- config (must match docker-compose.yml) ----------
INFLUX_URL="http://localhost:8086"
INFLUX_TOKEN="aegis-super-secret-token"
INFLUX_ORG="aegis"
INFLUX_BUCKET="events"
WAIT_SECONDS=30
# ------------------------------------------------------------

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

info()  { echo -e "${GREEN}[setup]${NC} $*"; }
warn()  { echo -e "${YELLOW}[setup]${NC} $*"; }
error() { echo -e "${RED}[setup]${NC} $*"; exit 1; }

# ── 1. Wait for InfluxDB to be reachable ──────────────────────
info "Waiting for InfluxDB at ${INFLUX_URL} ..."
elapsed=0
until curl -sf "${INFLUX_URL}/health" | grep -q '"status":"pass"'; do
    if [ $elapsed -ge $WAIT_SECONDS ]; then
        error "InfluxDB did not become healthy within ${WAIT_SECONDS}s. Is Docker running?"
    fi
    sleep 2
    elapsed=$((elapsed + 2))
    echo -n "."
done
echo ""
info "InfluxDB is healthy."

# ── 2. Confirm org exists ─────────────────────────────────────
info "Checking org '${INFLUX_ORG}' ..."
ORG_ID=$(curl -sf \
    -H "Authorization: Token ${INFLUX_TOKEN}" \
    "${INFLUX_URL}/api/v2/orgs?org=${INFLUX_ORG}" \
    | grep -o '"id":"[^"]*"' | head -1 | cut -d'"' -f4)

if [ -z "$ORG_ID" ]; then
    error "Org '${INFLUX_ORG}' not found. Did docker-compose initialise correctly?"
fi
info "Org found — id=${ORG_ID}"

# ── 3. Confirm bucket exists (create if missing) ──────────────
info "Checking bucket '${INFLUX_BUCKET}' ..."
BUCKET_ID=$(curl -sf \
    -H "Authorization: Token ${INFLUX_TOKEN}" \
    "${INFLUX_URL}/api/v2/buckets?org=${INFLUX_ORG}&name=${INFLUX_BUCKET}" \
    | grep -o '"id":"[^"]*"' | head -1 | cut -d'"' -f4)

if [ -z "$BUCKET_ID" ]; then
    warn "Bucket '${INFLUX_BUCKET}' not found — creating it ..."
    BUCKET_ID=$(curl -sf -X POST \
        -H "Authorization: Token ${INFLUX_TOKEN}" \
        -H "Content-Type: application/json" \
        "${INFLUX_URL}/api/v2/buckets" \
        -d "{\"orgID\":\"${ORG_ID}\",\"name\":\"${INFLUX_BUCKET}\",\"retentionRules\":[{\"type\":\"expire\",\"everySeconds\":604800}]}" \
        | grep -o '"id":"[^"]*"' | head -1 | cut -d'"' -f4)
    info "Bucket created — id=${BUCKET_ID}"
else
    info "Bucket already exists — id=${BUCKET_ID}"
fi

# ── 4. Write a test point to verify write access ──────────────
info "Writing a test data point ..."
HTTP_STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -H "Authorization: Token ${INFLUX_TOKEN}" \
    -H "Content-Type: text/plain; charset=utf-8" \
    "${INFLUX_URL}/api/v2/write?org=${INFLUX_ORG}&bucket=${INFLUX_BUCKET}&precision=ns" \
    --data-raw "honeypot_event,attacker_ip=127.0.0.1,event_type=test,device_id=setup-check detail=\"setup test\",count=1i,uptime_ms=0i")

if [ "$HTTP_STATUS" -eq 204 ]; then
    info "Test write succeeded (HTTP 204)."
else
    error "Test write failed (HTTP ${HTTP_STATUS}). Check token and bucket name."
fi

# ── 5. Summary ────────────────────────────────────────────────
echo ""
echo "============================================"
echo -e "  ${GREEN}AEGIS-C3 InfluxDB Setup Complete${NC}"
echo "============================================"
echo "  URL    : ${INFLUX_URL}"
echo "  Org    : ${INFLUX_ORG}  (id=${ORG_ID})"
echo "  Bucket : ${INFLUX_BUCKET}  (id=${BUCKET_ID})"
echo ""
echo "  Copy the following into backend/.env :"
echo "  INFLUX_URL=${INFLUX_URL}"
echo "  INFLUX_TOKEN=${INFLUX_TOKEN}"
echo "  INFLUX_ORG=${INFLUX_ORG}"
echo "  INFLUX_BUCKET=${INFLUX_BUCKET}"
echo "============================================"
