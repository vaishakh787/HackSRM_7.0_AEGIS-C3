# =============================================================
#  AEGIS-C3 — Events Route
#  POST /event  — receive and store honeypot detection events
#  POST /health — liveness check
# =============================================================

import os
from flask import Blueprint, request, jsonify
from influxdb_client import Point, WritePrecision
from datetime import datetime, timezone

from models.event_model import HoneypotEvent
from db.influx_client import get_write_api

events_bp = Blueprint("events", __name__)


@events_bp.route("/event", methods=["POST"])
def receive_event():
    # --- Parse ---
    data = request.get_json(silent=True)
    if not data:
        return jsonify({"error": "Request body must be valid JSON"}), 400

    # --- Validate ---
    try:
        evt = HoneypotEvent.from_dict(data)
    except ValueError as e:
        return jsonify({"error": str(e)}), 422

    # --- Write to InfluxDB ---
    try:
        point = (
            Point("honeypot_event")
            .tag("attacker_ip", evt.attacker_ip)
            .tag("event_type",  evt.event_type)
            .tag("device_id",   evt.device_id)
            .field("detail",    evt.detail)
            .field("count",     1)
            .field("uptime_ms", evt.uptime_ms if evt.uptime_ms is not None else 0)
            .time(datetime.now(timezone.utc), WritePrecision.NS)
        )

        get_write_api().write(
            bucket=os.getenv("INFLUX_BUCKET", "events"),
            record=point
        )
    except Exception as e:
        print(f"[Events] InfluxDB write failed: {e}")
        return jsonify({"error": "Database write failed", "detail": str(e)}), 500

    print(f"[Events] Stored: {evt.event_type:12s} | IP={evt.attacker_ip} | device={evt.device_id}")
    return jsonify({"status": "ok"}), 200