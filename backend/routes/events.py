from flask import Blueprint, request, jsonify
from influxdb_client import Point, WritePrecision
from datetime import datetime, timezone
from models.event_model import HoneypotEvent
from db.influx_client import get_write_api
import os

events_bp = Blueprint("events", __name__)

@events_bp.route("/event", methods=["POST"])
def receive_event():
    data = request.get_json(silent=True)
    if not data:
        return jsonify({"error": "invalid JSON"}), 400

    try:
        evt = HoneypotEvent.from_dict(data)
    except ValueError as e:
        return jsonify({"error": str(e)}), 422

    point = (
        Point("honeypot_event")
        .tag("attacker_ip", evt.attacker_ip)
        .tag("event_type",  evt.event_type)
        .tag("device_id",   evt.device_id)
        .field("detail",    evt.detail)
        .field("count",     1)
        .time(datetime.now(timezone.utc), WritePrecision.NS)
    )

    get_write_api().write(
        bucket = os.getenv("INFLUX_BUCKET", "events"),
        record = point
    )

    print(f"[Stored] {evt.event_type} from {evt.attacker_ip}")
    return jsonify({"status": "ok"}), 200