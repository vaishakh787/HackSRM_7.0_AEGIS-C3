# =============================================================
#  AEGIS-C3 — Flask Backend Entry Point
#  Run from the backend/ directory:
#    cd backend && python app.py
# =============================================================

from dotenv import load_dotenv
load_dotenv()  # loads backend/.env into os.environ before anything else

import os
from flask import Flask, jsonify
from routes.events import events_bp

app = Flask(__name__)
app.register_blueprint(events_bp)


@app.route("/health", methods=["GET"])
def health():
    """Simple liveness check — use to confirm backend is reachable."""
    return jsonify({"status": "ok", "service": "aegis-c3-backend"}), 200


if __name__ == "__main__":
    port = int(os.getenv("FLASK_PORT", 5000))
    print("=" * 50)
    print("  AEGIS-C3 Backend Starting")
    print(f"  Listening on http://0.0.0.0:{port}")
    print(f"  InfluxDB URL: {os.getenv('INFLUX_URL', 'NOT SET')}")
    print(f"  InfluxDB Org: {os.getenv('INFLUX_ORG', 'NOT SET')}")
    print(f"  InfluxDB Bucket: {os.getenv('INFLUX_BUCKET', 'NOT SET')}")
    token = os.getenv("INFLUX_TOKEN", "")
    print(f"  InfluxDB Token: {'SET (' + str(len(token)) + ' chars)' if token else 'NOT SET ⚠️'}")
    print("=" * 50)
    app.run(host="0.0.0.0", port=port, debug=True)