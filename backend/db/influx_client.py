import os
from influxdb_client import InfluxDBClient, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

_client    = None
_write_api = None

def get_write_api():
    global _client, _write_api
    if _write_api is None:
        _client = InfluxDBClient(
            url   = os.getenv("INFLUX_URL",   "http://localhost:8086"),
            token = os.getenv("INFLUX_TOKEN", ""),
            org   = os.getenv("INFLUX_ORG",   "aegis"),
        )
        _write_api = _client.write_api(write_options=SYNCHRONOUS)
    return _write_api