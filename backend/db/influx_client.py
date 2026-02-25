# =============================================================
#  AEGIS-C3 — InfluxDB Client
#  Lazy singleton — connection created on first write
# =============================================================

import os
from influxdb_client import InfluxDBClient, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

_client    = None
_write_api = None


def get_write_api():
    """
    Returns a singleton InfluxDB write API.
    Reads credentials from environment variables (set via .env).
    """
    global _client, _write_api

    if _write_api is None:
        url   = os.getenv("INFLUX_URL",   "http://localhost:8086")
        token = os.getenv("INFLUX_TOKEN", "")
        org   = os.getenv("INFLUX_ORG",   "aegis")

        if not token:
            raise RuntimeError(
                "[InfluxDB] INFLUX_TOKEN is not set. "
                "Copy backend/.env.example to backend/.env and add your token."
            )

        _client = InfluxDBClient(url=url, token=token, org=org)
        _write_api = _client.write_api(write_options=SYNCHRONOUS)
        print(f"[InfluxDB] Connected to {url} | org={org}")

    return _write_api