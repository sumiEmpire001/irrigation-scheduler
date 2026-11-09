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

## C++ Arduino / ESP32 implementation

The existing Python simulation remains available. An independent header-only
C++11 controller lives at `arduino/IrrigationScheduler/src/IrrigationScheduler.h`.
It implements moisture start/stop thresholds, dry-tank interlock, max continuous
runtime, cooldown and monotonic timestamp validation. Invalid readings request
pump OFF and must be investigated.

The Arduino `IndicatorDemo` uses a **potentiometer** to simulate soil moisture,
a tank-available switch connected to ground, and an LED to indicate a pump request.
It does **not** switch a real pump. Pin mappings: Arduino Uno A0/D7/D8;
classic ESP32 GPIO34/4/18. Install `arduino/IrrigationScheduler` as a sketchbook
library and open its example.

### Build and test C++ on a computer
```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

GitHub Actions checks GCC and Clang compilation. Actual Arduino/ESP32 firmware
compilation, long-running `millis()` rollover, calibrated moisture sensors,
electrical isolation, fail-safe relays, physical water-level sensing and
independent overrun cutoff are **not yet tested**. Never use a GPIO to directly
drive a motor or connect this prototype to unattended watering equipment.
