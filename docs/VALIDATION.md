# Validation record

Checked on 2026-09-10 using the locally installed Arduino CLI and ESP32 + Bluepad32 4.1.0 board package.

## Compilation

Board target: `esp32-bluepad32:esp32:esp32` (generic original ESP32).

| Sketch | Result | Flash usage | Global RAM usage |
| --- | --- | --- | --- |
| `SingleStickctrl/SingleStickctrl.ino` | Passed | 719,033 bytes / 1,310,720 | 87,212 bytes / 327,680 |
| `TankdrvCtrl/TankdrvCtrl.ino` | Passed | 719,005 bytes / 1,310,720 | 87,212 bytes / 327,680 |

Commands, run from the repository root:

```sh
arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 SingleStickctrl
arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 TankdrvCtrl
```

## Documentation checks

- Verified all six GPIO constants in both sketches against the tutorial.
- Verified A/right and B/left using each sketch's motor-control calls.
- Confirmed the sketch copies in the CAD folder match the root sketch files byte-for-byte.
- Verified relative file links in the README and tutorial resolve locally.
- Visually reviewed the final generated image: six GPIO pairs, both motor output pairs, three distinct main power nets, capacitor polarity and parallel connection, and the isolated switched 18650 loop.
- Kept original code, videos, and CAD files unchanged. Original sketch whitespace and CAD formatting are preserved.

Compilation does not verify physical wiring, motor direction, thermal performance, controller pairing, or failsafe behavior. No board was flashed and no hardware test was performed in this session. The stale-packet timeout limitation is documented in the tutorial.
