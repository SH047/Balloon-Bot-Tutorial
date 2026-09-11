# Balloon Bot

Bluetooth gamepad-controlled robot built with an ESP32, Bluepad32, an L298N dual H-bridge motor driver, and two DC drive motors.

This repository includes two controller modes, mechanical CAD files, original tutorial and demonstration videos, and a wiring reference covering the requested 1000 µF capacitor and independent 18650-powered auxiliary motor.

## Wiring diagram

![Balloon Bot ESP32 and L298N wiring diagram](docs/images/balloon-bot-wiring.png)

> [!IMPORTANT]
> The diagram and table follow the supplied sketches: **Motor A is the right motor** and **Motor B is the left motor**. Check the labels printed on your own ESP32 and L298N boards before applying power.

## ESP32 to L298N connections

| ESP32 pin | L298N pin | Function |
| --- | --- | --- |
| GPIO 13 | IN1 | Right motor direction 1 |
| GPIO 26 | IN2 | Right motor direction 2 |
| GPIO 32 | ENA | Right motor PWM / speed control |
| GPIO 14 | IN3 | Left motor direction 1 |
| GPIO 27 | IN4 | Left motor direction 2 |
| GPIO 33 | ENB | Left motor PWM / speed control |

Remove the black jumper caps from **ENA** and **ENB** before connecting GPIO 32 and GPIO 33. These enable pins receive PWM from the ESP32.

### Drive motors

- L298N `OUT1` and `OUT2` → right drive motor (Motor A)
- L298N `OUT3` and `OUT4` → left drive motor (Motor B)
- If a wheel turns in the wrong direction, disconnect power and swap that motor's two output wires.

## Main power wiring

Make every connection with the battery and USB cable disconnected.

| Source | Destination |
| --- | --- |
| 12V battery positive | L298N `12V / VS` and buck converter `IN+` |
| 12V battery negative | L298N `GND` and buck converter `IN−` |
| Buck converter `OUT+` set to 5.0V | ESP32 `5V / VIN` input, if supported by your board |
| Buck converter `OUT−` | ESP32 `GND` |
| ESP32 `GND` | L298N `GND` common ground |

The ESP32, L298N, buck converter, and main battery must share a common ground. Set and measure the buck converter output at **5.0V before connecting the ESP32**. Never connect the 12V battery directly to an ESP32 power pin, and never connect 5V to the ESP32 `3V3` pin.

The diagram leaves the L298N `5V` terminal disconnected. This assumes the module's separate `5V-EN` regulator jumper is fitted and the onboard regulator is compatible with the battery's fully charged voltage. L298N modules vary, so verify your module. Do not connect an enabled onboard 5V regulator output to the buck converter output.

## 1000 µF capacitor

Install a polarized **1000 µF electrolytic capacitor rated for 10V or higher** close to the ESP32:

```text
Buck OUT+ (5.0V) ────────┬──────── ESP32 5V
                         │ +
                    1000 µF
                     ≥ 10V
                         │ −
Buck OUT− (GND) ─────────┴──────── ESP32 GND
```

- Capacitor positive (`+`) → 5V
- Capacitor negative (`−`, normally marked with a stripe) → GND
- Connect it **in parallel across 5V and GND**, never in series.
- Keep the capacitor leads short.

## Separate 18650 motor circuit

The auxiliary motor is a separate manual circuit and has no connection to the ESP32 or L298N:

```text
Protected 18650 (+) ─── SPST on/off switch ─── Auxiliary motor (+)
Protected 18650 (−) ───────────────────────── Auxiliary motor (−)
```

Use a motor rated for a Li-ion cell's **3.7V nominal / 4.2V fully charged** voltage. The protected cell, holder, switch, and wiring must safely carry the motor's starting and stall current. Charge the cell only with a suitable Li-ion charger.

## Controller modes

### Single-stick arcade control

[SingleStickctrl.ino](SingleStickctrl/SingleStickctrl.ino) uses the left stick for throttle and steering, mixing both values into left- and right-wheel commands.

### Two-stick tank control

[TankdrvCtrl.ino](TankdrvCtrl/TankdrvCtrl.ino) uses the right stick for Motor A/right and the left stick for Motor B/left.

Both sketches use a deadzone and capped PWM output. Read the documented [failsafe limitation](docs/TUTORIAL.md#code-behavior-and-limitation) before testing.

## Arduino setup

1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Follow the [Bluepad32 Arduino setup guide](https://bluepad32.readthedocs.io/en/latest/plat_arduino/).
3. Install the ESP32 and ESP32 + Bluepad32 board packages.
4. Select your compatible original ESP32 board under **Tools → Board → ESP32 + Bluepad32 Arduino**.
5. Open one controller sketch, select the USB port, and upload it.
6. Open Serial Monitor at **115200 baud** and pair the controller.

For PS4/PS5-style controllers that use Bluetooth Classic, use an original ESP32 such as an ESP32-WROOM-32 board. ESP32-S3 and ESP32-C3 boards do not provide Bluetooth Classic.

## Videos

- [Code training tutorial](videos/Ballon%20bot%20Code%20training%20.mov)
- [Balloon Bot demonstration](videos/Demo%20Ballon%20bot%20.mp4)
- [Download both original recordings from the GitHub release](https://github.com/SH047/Balloon-Bot-Tutorial/releases/tag/videos-v1)

## Repository contents

```text
Balloon-Bot-Tutorial/
├── SingleStickctrl/       Single-stick arcade control sketch
├── TankdrvCtrl/           Two-stick tank control sketch
├── Ballon BotCAD /        STL and DXF mechanical parts
├── docs/                  Full tutorial, validation, and wiring assets
├── videos/                Original tutorial and demonstration recordings
└── README.md              Project overview and wiring guide
```

## Documentation

Both sketches compile successfully against the ESP32 + Bluepad32 board package. See [VALIDATION.md](docs/VALIDATION.md) for recorded checks and [TUTORIAL.md](docs/TUTORIAL.md) for detailed assembly, testing, troubleshooting, and safety notes.

## Author

Created by [Shreyas S Rai](https://github.com/SH047).
