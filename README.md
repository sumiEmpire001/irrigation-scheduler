# irrigation-scheduler

A deterministic soil-moisture irrigation simulation with start/stop hysteresis, dry-tank interlock, maximum continuous runtime and cooldown. Inputs are explicit readings and timestamps; no real sensor drivers, pump controls, persistent state, or automatic watering are provided.

## Run tests

Requires Python 3.10+ with no external dependencies.

```bash
python -m unittest discover -s tests -v
```

## Example

```python
from irrigation_scheduler import IrrigationController
controller = IrrigationController()
print(controller.ingest(0, 20, tank_available=True))
```

GitHub Actions runs the test suite on Python 3.10, 3.12, and 3.13. Real hardware integration and deployment remain future work.
