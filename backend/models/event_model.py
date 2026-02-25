from dataclasses import dataclass
from typing import Optional

@dataclass
class HoneypotEvent:
    attacker_ip: str
    event_type:  str          # "scan" | "brute_force"
    detail:      str
    device_id:   str
    uptime_ms:   Optional[int] = None

    @classmethod
    def from_dict(cls, data: dict) -> "HoneypotEvent":
        required = {"attacker_ip", "event_type", "detail", "device_id"}
        missing  = required - data.keys()
        if missing:
            raise ValueError(f"Missing fields: {missing}")
        return cls(
            attacker_ip = data["attacker_ip"],
            event_type  = data["event_type"],
            detail      = data["detail"],
            device_id   = data["device_id"],
            uptime_ms   = data.get("uptime_ms"),
        )