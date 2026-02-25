# =============================================================
#  AEGIS-C3 — Event Model
#  Validates and structures incoming honeypot events
# =============================================================

from dataclasses import dataclass
from typing import Optional


@dataclass
class HoneypotEvent:
    attacker_ip: str
    event_type:  str           # expected: "scan" | "brute_force"
    detail:      str
    device_id:   str
    uptime_ms:   Optional[int] = None   # relative ESP32 uptime; backend adds wall clock

    @classmethod
    def from_dict(cls, data: dict) -> "HoneypotEvent":
        required = {"attacker_ip", "event_type", "detail", "device_id"}
        missing  = required - data.keys()
        if missing:
            raise ValueError(f"Missing required fields: {sorted(missing)}")

        event_type = data["event_type"]
        if event_type not in ("scan", "brute_force"):
            # Accept unknown types but warn — future-proof for added rules
            print(f"[EventModel] Warning: unknown event_type '{event_type}'")

        return cls(
            attacker_ip = str(data["attacker_ip"]),
            event_type  = str(event_type),
            detail      = str(data["detail"]),
            device_id   = str(data["device_id"]),
            uptime_ms   = int(data["uptime_ms"]) if data.get("uptime_ms") is not None else None,
        )