"""Conservative soil-moisture irrigation controller simulation."""
from math import isfinite

class IrrigationController:
    def __init__(self, start_below=30, stop_above=55, max_run_seconds=600, cooldown_seconds=300):
        if any(type(x) not in (int, float) or not isfinite(x) for x in (start_below, stop_above, max_run_seconds, cooldown_seconds)):
            raise ValueError("configuration must be finite numbers")
        if not 0 <= start_below < stop_above <= 100 or max_run_seconds <= 0 or cooldown_seconds < 0:
            raise ValueError("invalid irrigation settings")
        self.start_below = start_below
        self.stop_above = stop_above
        self.max_run_seconds = max_run_seconds
        self.cooldown_seconds = cooldown_seconds
        self.running_since = None
        self.last_stop = None
        self.last_timestamp = None

    def ingest(self, timestamp, moisture, tank_available=True):
        if type(timestamp) not in (int, float) or not isfinite(timestamp) or timestamp < 0:
            raise ValueError("timestamp must be nonnegative finite seconds")
        if self.last_timestamp is not None and timestamp <= self.last_timestamp:
            raise ValueError("timestamps must increase")
        if type(moisture) not in (int, float) or not isfinite(moisture) or not 0 <= moisture <= 100:
            raise ValueError("moisture must be between 0 and 100")
        if type(tank_available) is not bool:
            raise ValueError("tank_available must be bool")
        reason = "idle"
        if self.running_since is not None:
            if not tank_available:
                reason = "tank_empty"
            elif timestamp - self.running_since >= self.max_run_seconds:
                reason = "max_runtime"
            elif moisture >= self.stop_above:
                reason = "target_reached"
            else:
                reason = "watering"
            if reason != "watering":
                self.running_since = None
                self.last_stop = timestamp
        elif not tank_available:
            reason = "tank_empty"
        elif self.last_stop is not None and timestamp - self.last_stop < self.cooldown_seconds:
            reason = "cooldown"
        elif moisture < self.start_below:
            self.running_since = timestamp
            reason = "watering"
        self.last_timestamp = timestamp
        return {"timestamp": timestamp, "moisture": moisture,
                "pump_on": self.running_since is not None, "reason": reason}
