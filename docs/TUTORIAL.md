# ESP32 Balloon Bot: wiring and setup

This written companion follows the two original sketches and the [code-training recording](../videos/Ballon%20bot%20Code%20training%20.mov). It adds the requested 1000 µF capacitor and a separate, manually switched 18650 motor circuit.

## 1. Parts

- An original ESP32 development board with Bluetooth Classic, such as an ESP32-WROOM-32 DevKit, and a USB data cable.
- A compatible Bluetooth gamepad; the sketches print a PS5-controller connection message.
- An L298N dual H-bridge module and two drive motors compatible with the supply and driver.
- A suitable nominal 12V battery, wiring, connectors, and overcurrent protection sized for the motors and wiring.
- A buck converter that accepts the battery's **fully charged** voltage and provides regulated 5V with enough current for the ESP32.
- One polarized **1000 µF / 10V or higher** electrolytic capacitor.
- For the independent motor: one protected 18650 Li-ion cell in a holder, a DC-rated on/off switch, and a motor compatible with 3.7V nominal / 4.2V fully charged operation.
- Mechanical parts from the [CAD directory](../Ballon%20BotCAD%20/).

For a DualSense/PS5 controller, use an original ESP32 with Bluetooth Classic. ESP32-S3 and ESP32-C3 do not provide the required Classic transport. See the [Bluepad32 compatibility FAQ](https://bluepad32.readthedocs.io/en/latest/FAQ/).

## 2. Wiring reference

![Complete Balloon Bot wiring reference](images/balloon-bot-wiring.png)

This is a **functional connection diagram**, not the physical header layout of a particular ESP32 or L298N board. Find the printed pin labels on your hardware. Matching net labels in the main power section mean those points must be wired together; the labels do not imply wireless connections.

### ESP32 to L298N

Both sketches declare the same six pins. Their motor-control calls assign **A to the right wheel and B to the left wheel**, correcting the opposite wheel labels in the original written notes.

| ESP32 pin | L298N pin | Function in the supplied code |
| --- | --- | --- |
| GPIO 13 | IN1 | Motor A — right, direction input 1 |
| GPIO 26 | IN2 | Motor A — right, direction input 2 |
| GPIO 32 | ENA | Motor A — right, PWM / speed |
| GPIO 14 | IN3 | Motor B — left, direction input 1 |
| GPIO 27 | IN4 | Motor B — left, direction input 2 |
| GPIO 33 | ENB | Motor B — left, PWM / speed |

Remove the black jumper caps from **ENA and ENB**, then connect GPIO 32 and GPIO 33 to the corresponding enable input pins. Do not connect them to the adjacent 5V jumper posts.

| L298N output | Motor connection |
| --- | --- |
| OUT1 and OUT2 | The two terminals of Motor A, the **right** drive motor |
| OUT3 and OUT4 | The two terminals of Motor B, the **left** drive motor |

If a wheel rotates opposite to the intended direction, turn power off and swap that motor's two output wires. Keep the A/right and B/left channels consistent with the sketches. The tank and arcade sketches handle Y-axis signs differently, so check direction whenever switching sketches.

## 3. Main battery, buck converter, and common ground

Make all connections with the battery and USB disconnected.

| From | To |
| --- | --- |
| Main battery positive | L298N `12V` / `VS` terminal and buck `IN+` |
| Main battery negative | L298N `GND` and buck `IN−` |
| Buck `OUT+`, adjusted to 5.0V | ESP32 board's documented **5V power input** |
| Buck `OUT−` | ESP32 `GND` and the main common ground |
| ESP32 `GND` | L298N `GND` |

The main common ground includes battery negative, L298N GND, buck IN−/OUT−, and ESP32 GND. This guide assumes a typical non-isolated buck converter. Verify your converter's terminal markings. Route motor-current returns directly to the supply/driver connection instead of through an ESP32 jumper wire.

Measure the buck output before attaching the ESP32. **Never apply 12V to the ESP32 5V pin, or 5V to its 3V3 pin.** A pin marked VIN is usable only if your development board documents it as the appropriate 5V input. Disconnect the external 5V supply before powering the board over USB, following the [Espressif DevKitC power guide](https://documentation.espressif.com/api/resource/path/docs/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html).

### L298N 5V terminal and regulator jumper

The drawing leaves the L298N **5V terminal disconnected** from the buck and ESP32. This assumes the module's onboard regulator supplies its logic power with the separate **5V-EN regulator jumper fitted**.

Confirm the module's allowed regulator-input range against the actual battery voltage, including fully charged voltage. A nominal 12V battery can exceed 12V when charged; the label alone does not establish compatibility. L298N boards vary. The [Handson Technology module guide](https://handsontec.com/dataspecs/module/L298N%20Motor%20Driver.pdf) illustrates the onboard regulator, its jumper, and external-5V operation.

If your module requires external logic power at the chosen battery voltage, remove its **5V-EN regulator jumper** and provide regulated 5V to its logic terminal according to that module's documentation. That is an alternative to the diagram's disconnected-5V arrangement. Do not join an enabled regulator's 5V output to the buck output. In either configuration, the **ENA and ENB PWM jumpers remain removed**.

### Add the 1000 µF capacitor

Connect C1 **in parallel across the buck's 5V output**, physically close to the ESP32 power pins:

```text
Buck OUT+ (5.0V) ────────┬──────── ESP32 5V
                        │ +
                   C1 1000 µF
                      ≥ 10V
                        │ −
Buck OUT− (GND) ────────┴──────── ESP32 GND
```

The capacitor's positive lead goes to 5V; its negative lead (usually identified by the stripe) goes to GND. Keep both leads short. Do not put the capacitor in series with the ESP32 supply. If adding another 1000 µF capacitor, connect it across the same rails with the same polarity and confirm that the buck supports the total output capacitance. Bulk capacitance can help with brief supply dips, but does not replace adequate supply current and good wiring.

## 4. Separate 18650 → switch → motor

The boxed auxiliary circuit is independent of the ESP32 and L298N:

```text
Protected 18650 (+) ─── SPST on/off switch ─── Motor (+)
Protected 18650 (−) ───────────────────────── Motor (−)
```

Use a motor rated for the cell's 4.2V full-charge voltage, and a cell, protection circuit, switch, and wires that can handle its starting/stall current. Use a suitable holder and charger for the cell. This motor is operated by the physical switch only; the supplied sketches cannot start or stop it. No common-ground connection to the main robot circuit is needed for this independent loop. Keep it switched off during drive-system setup.

## 5. Upload one sketch

Install the ESP32 and ESP32 + Bluepad32 board packages using the [official Bluepad32 Arduino setup guide](https://bluepad32.readthedocs.io/en/latest/plat_arduino/). In Arduino IDE's Additional Boards Manager URLs, add:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
```

Select your board under **Tools → Board → ESP32 + Bluepad32 Arduino**, then select its USB port. Open and upload **one** of these:

- [SingleStickctrl.ino](../SingleStickctrl/SingleStickctrl.ino): left stick provides throttle and steering. The code negates Y for throttle, then computes `leftPower = throttle + turn` and `rightPower = throttle - turn`.
- [TankdrvCtrl.ino](../TankdrvCtrl/TankdrvCtrl.ino): right-stick Y controls Motor A/right; left-stick Y controls Motor B/left. This sketch passes the raw Y-axis signs to the motor function.

Keep each sketch in its matching folder. Do not combine them into one Arduino sketch. Open Serial Monitor at **115200 baud**, put the controller into its documented Bluetooth pairing mode, and watch for the connection callback. Use the controller manufacturer's pairing instructions for your model.

## 6. Test the controls

1. Keep wheels clear of the ground and the separate auxiliary motor off.
2. Check polarity, grounds, the 5V measurement, capacitor orientation, and all three L298N jumper settings.
3. Power the main circuit, pair the controller, and move the sticks gently.
4. Confirm the right and left channels, then check forward/reverse and steering. Remove power before changing motor wires.
5. Release the sticks and verify that both wheel motors stop.
6. Turn off the controller and verify disconnect stopping. Test with an accessible physical power cutoff.
7. Test the independent 18650 motor separately with its own switch.

The code uses `DEADZONE = 15`, `MIN_PWM = 60`, and `MAX_PWM = 120`; motor commands are constrained to −512…512. PWM limits do not guarantee safe stall current, motor voltage, or driver temperature. Avoid sustained stalls and check the hardware ratings.

### Code behavior and limitation

Both sketches stop the motors at startup, inside the deadzone, and in the controller-disconnect callback. They declare `COMMAND_TIMEOUT = 300`, but update `lastCommandTime` on every loop that finds a connected controller, without checking whether a **new input packet** arrived. Consequently, the timeout is not a reliable stale-packet failsafe while the controller remains marked connected. The sketches also process only `myControllers[0]`.

These original sketches are retained unchanged for reference. A future failsafe improvement should update the command timestamp only when fresh, valid gamepad data arrives and be verified with packet-loss and disconnect tests. Treat the physical power cutoff as necessary during initial testing.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| `Bluepad32.h` is missing | Use the ESP32 + Bluepad32 board package and board selection |
| PS5 controller will not pair | Original ESP32 / Bluetooth Classic support and controller pairing mode |
| Motors run at fixed speed | ENA/ENB jumpers removed; GPIO 32/33 on the enable inputs |
| Left/right controls are reversed | OUT1/2 = right; OUT3/4 = left, matching the code |
| A motor rotates backward | With power off, reverse that motor's two wires |
| ESP32 resets as motors start | Buck current capability, 5V wiring, common-ground routing, and C1 polarity/placement |
| L298N is powered but unresponsive | Logic 5V supply/regulator configuration and common ground |
| Auxiliary motor ignores the gamepad | Expected: it is controlled only by its separate switch |

## Image and verification notes

The PNG was created and corrected with the built-in image-generation tool. The [initial prompt](images/wiring-image-prompt.txt) and [final correction prompt](images/wiring-image-edit-prompt.txt) are included for reproducibility. The signal labels and wheel assignments were checked against both sketches. The final power connections use explicit net labels to avoid ambiguous crossings.

Documentation review does not establish that the physical hardware has been tested. Verify your actual board variant, battery, converter, capacitor, and motor ratings before assembly.
