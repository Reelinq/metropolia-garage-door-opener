# Garage Door Opener

A garage door opener powered by Raspberry Pi Pico W. A stepper motor moves the door along a belt, a rotary encoder measures the movement, and two limit switches mark the ends of the travel. The door can be operated locally with buttons and remotely over MQTT, and it reports its status to an MQTT broker.

---

## Features

- **Self-calibration**: The travel length is measured in encoder ticks by driving the door between the two limit switches
- **Safe end stops**: The motor is switched off the moment a limit switch closes, before the door can hit the switch body
- **Stuck detection**: If the motor runs but the encoder reports no movement, a watchdog resets the device and the door is reported as stuck
- **Power-loss recovery**: Calibration and door position are saved to EEPROM and restored on boot
- **Local control**: Three buttons and three LEDs show and control the state of the door
- **Remote control over MQTT**: The same operations as the buttons, with a response to every command
- **Status reporting**: Door state, error state and calibration state are published whenever any of them changes

---

## Setup and Flashing

### 1. Hardware that you need

| Item | Quantity |
|---|---|
| Raspberry Pi Pico W + JWH Add-on Board v1.4 | 1 |
| Raspberry Pi Debug Probe | 1 |
| 28BYJ-48 5V DC Stepper Motor | 1 |
| ULN2003 Stepper Motor Driver Board | 1 |
| Crowtal I2C EEPROM v2.0 | 1 |
| Garage door housing (3D printed) | 1 |
| Garage door belt | 1 |
| USB Type-A to Micro USB cable | 2 |
| Grove 4-pin buckled cable | 3 |
| 3-pin JST to 3-pin JST cable | 1 |
| 3-pin JST to 0.1-inch header male cable | 1 |
| Crowtail- Encoder 2.0 Rotary encoder | 1 |
| SS-5GLT Limit Microswitch | 2 |

> **Note:** Never turn the stepper motor or move the belt by hand!

### 2. Wiring

Connect the components to the Raspberry Pi Pico Add-On Board as follows.

**PicoProbe**
1. Connect the JST-JST cable to the connector marked **D** on the PicoProbe and the other end to the debug connector next to the WiFi module.
2. Connect the JST-Male cable to the connector marked **U** on the PicoProbe and connect the loose ends: orange to GP1, yellow to GP0, and black to GND.
3. Connect a USB-A to Micro USB cable to the PicoProbe and plug the other end into your PC.

**Garage door**
- Glue limit switches, stepper motor, and rotary encoder to the door housing, then add belt.
- Connect limit switches to the ADC_1
- Connect rotary encoder to the UART_1

**Stepper motor**
1. Connect the motor to the driver board using the 5-pin cable.
2. Connect the driver board's VDD, IN1–IN4, and GND pins to connector J13 on the Add-On Board.

**EEPROM**
- Connect the EEPROM to the I2C_0 connector using a Grove cable.

**Power**
- Connect the Add-On Board to your PC using the second USB-A to Micro USB cable.

### 3. Prerequisites (Linux)

Install the required tools:

```bash
sudo apt install cmake ninja-build gcc-arm-none-eabi opencod
```

Clone the Pico SDK and set the environment variable:

```bash
git clone https://github.com/raspberrypi/pico-sdk.git --recurse-submodules
export PICO_SDK_PATH=/path/to/pico-sdk
```

The project also needs the Paho MQTT embedded C library in the project root:

```bash
git clone https://github.com/eclipse/paho.mqtt.embedded-c.git
```

### 4. Configure

**Wi-Fi credentials** are read from environment variables when CMake configures the project:

```bash
export WIFI_SSID='your_network_name'
export WIFI_PASSWORD='your_password'
```

> **Note:** The Pico W only supports 2.4 GHz networks with WPA2-PSK. If you change the credentials later, delete the `build` folder, because the old values stay compiled in until CMake configures again.

**Broker address** is set in `src/Config.h`:

```c
#define BROKER_IP "192.168.0.10" // must be an IP address, hostnames are not supported
#define BROKER_PORT 1883
```

Other timings and pin numbers are also in `Config.h`. See the [Configuration](#configuration) section.

### 5. Build and Flash (Linux)

Run the build and flash script from the project root:

```bash
chmod +x bps.sh
./bps.sh
```

The script builds the project and flashes it to the Pico W through the debug probe. It uses `sudo` for OpenOCD, so it asks for your password. The board resets and starts running automatically once flashing is complete.

> **Note:** Set `WIFI_SSID` and `WIFI_PASSWORD` in the same terminal before running the script (see [Configure](#4-configure)).

### 6. Read the output

Debug output goes to UART at 115200 baud. Open it with picocom:

```bash
picocom -b 115200 /dev/ttyACM0
```

(Change the device name to match your setup. Exit picocom with `Ctrl+A`, then `Ctrl+X`.)

---

## How does device work

### Configuration

All settings are in `src/Config.h`:

```c
#define LED_BLINK_MS 250          // Time between LED toggles (full blink = 500 ms)
#define BUTTON_POLL_MS 20         // Button sampling interval, also works as the debounce
#define MOVE_WD_MS 2000           // Motor may run this long without an encoder tick before the door counts as stuck
#define CALIBRATION_WD_MS 2000    // Same limit while calibrating
#define SETTLE_MS 100             // Wait for the belt to settle before reading the last ticks
#define ROT_QUEUE_SIZE 10         // Encoder ticks buffered between two reads
```

The file also holds the pin numbers, the EEPROM settings, the broker address and the MQTT topics.

### Local operation

| Input | Action |
|---|---|
| **SW0 + SW2** (pressed together) | Start calibration |
| **SW1** | Toggle the door (see below) |

SW1 works like this:

| Door is | SW1 does |
|---|---|
| Closed | Starts opening |
| Open | Starts closing |
| Opening or closing | Stops |
| Stopped halfway | Starts moving in the opposite direction from before the stop |

SW1 does nothing while the door is not calibrated.

### LEDs

| LED | Meaning |
|---|---|
| LED0 | On when the door is closed |
| LED1 | On when the door is open |
| LED2 | On when calibrated, off when not calibrated, **blinking when the door is stuck** |

### Door States

The door state comes from the position counted by the encoder:

```
NOT CALIBRATED  ◄──────────────────────────────┐
        │  (SW0 + SW2, or "calibrate")         │
        ▼                                      │
   CALIBRATING ── (failure) ───────────────────┤
        │  (success)                           │
        ▼                                      │
   CALIBRATED                                  │
 Closed ◄──► In between ◄──► Open              │
        │                                      │
        └── (no encoder ticks for 2 s) ────────┘
            watchdog reset → door stuck
```

- **Closed**: position is 0
- **Open**: position equals the calibrated length
- **In between**: anything else. A door that is not calibrated is also reported as "In between", since its position is unknown.

### Calibration

Calibration is started with **SW0 + SW2** or with the remote `calibrate` command.

The process:
1. The door is stopped and marked as not calibrated.
2. The motor runs towards the closed end until the closed limit switch closes. The position is set to 0.
3. The motor runs towards the open end until the open limit switch closes. The encoder ticks are counted on the way.
4. After a short settle time the last ticks are added. If the total is greater than 0, it is stored as the length of the travel.
5. The result is saved to EEPROM.

The motor stops as soon as the limit switch closes, so the door never hits the body of the switch. If the encoder counted the wrong way (total is not positive), calibration fails and the door stays not calibrated.

### Rotary encoder

The encoder has 20 detents per turn, so it detects the turning in 18 degree steps. An interrupt fires on the rising edge of signal A, and signal B tells the direction: B low means clockwise (+1) and B high means counter-clockwise (−1). The ticks are passed from the interrupt to the main program through a queue.

### Stuck detection

While the motor runs, a watchdog timer is restarted every time the encoder reports movement. If the motor runs for `MOVE_WD_MS` without a single tick, the watchdog resets the device. A marker in a watchdog scratch register tells the boot code that the reset happened while the motor was moving. After the reboot:

1. The error state is set to **Door stuck**.
2. The door goes to **not calibrated** and this is saved.
3. LED2 blinks and the status is published over MQTT.

A successful calibration clears the error.

### EEPROM Layout

The state of the door is saved as a 8-byte record at `STORAGE_ADDR` (defined in `Config.h`). Values are stored big-endian with an inverted checksum, so a blank or all-zero EEPROM is never accepted.

| Offset | Size | Contents |
|---|---|---|
| +0 | 2 bytes | Length of the travel in encoder ticks (0 = not calibrated) |
| +2 | 2 bytes | Door position in ticks |
| +4 | 2 bytes | Direction of the last movement, stored as direction + 1 (0 = closing, 1 = none, 2 = opening) |
| +6 | 2 bytes | Check value: `~(length + position + direction)` |

The record is saved after every calibration and every time the motor stops. If the check fails, the door starts as not calibrated.

### Power Loss Recovery

| State at power loss | Recovery behaviour |
|---|---|
| Door stopped | Calibration and position are restored from EEPROM |
| Door moving | The position from the last stop is restored. The first limit switch the door reaches corrects the position |
| Calibrating | The previous calibration is restored, or the door stays not calibrated if there was none |
| Stuck | Door is not calibrated, the error is reported again after the reboot |

The direction of the last movement is saved too, so a door stopped halfway still moves in the opposite direction after a reboot.

### MQTT

The device connects to the broker at boot. It tries up to 5 times, and if the broker cannot be reached, remote control stays off until the next reboot. Local operation works without the network.

| Topic | Direction | Purpose |
|---|---|---|
| `garage/door/status` | published | Door, error and calibration state |
| `garage/door/command` | subscribed | Remote commands |
| `garage/door/response` | published | Result of every command |

**Commands** (send the text as the message payload):

| Command | Same as | Notes |
|---|---|---|
| `calibrate` | SW0 + SW2 | Blocks the device until calibration is finished |
| `toggle` | SW1 | Fails if the door is not calibrated |

**Responses:**

```json
{"result":"ok"}
{"error":"Calibration failed"}
{"error":"Not calibrated"}
{"error":"Unknown command"}
```

**Status** is published whenever door state, error state or calibration state changes. If a publish fails, it is retried on the next loop.

```json
{"door":"Closed","error":"Normal","calibration":"Calibrated"}
```

| Field | Values |
|---|---|
| `door` | `Closed`, `Open`, `In between` |
| `error` | `Normal`, `Door stuck` |
| `calibration` | `Calibrated`, `Not calibrated` |

To try it with Mosquitto:

```bash
mosquitto_sub -h <broker_ip> -t "garage/door/#" -v
mosquitto_pub -h <broker_ip> -t garage/door/command -m toggle
```

> **Note:** The client ID of the device must be unique on the broker. Two clients with the same ID disconnect each other.

---

## Project Structure

```
garage-door-opener/
├── CMakeLists.txt
├── main.cpp
├── paho.mqtt.embedded-c/
└── src/
    ├── Config.h
    ├── hardware/   Stepper, RotaryEncoder, LimitSwitch, Led, Button, Eeprom
    ├── door/       Door, Calibration, Controller, DoorStorage
    ├── remote/     RemoteControl, Mqtt
    └── net/        IPStack, Countdown (network layer from the course template)
```

| Class | Responsibility |
|---|---|
| `Stepper` | Drives the motor coils and keeps the direction of movement |
| `RotaryEncoder` | Decodes turn direction in an interrupt and handles the stuck watchdog |
| `LimitSwitch` | Reads one end switch |
| `Button`, `Led` | Debounced input and LED modes (off, on, blink) |
| `Eeprom` | Reads and writes the I2C EEPROM |
| `Door` | Model of the door: position, length, calibration, error and direction logic |
| `Calibration` | Runs the calibration sequence |
| `DoorStorage` | Saves and restores the door state in EEPROM |
| `Controller` | Combines motor, encoder and switches. Starts, stops and toggles the door |
| `Mqtt` | MQTT connection, subscription and publishing |
| `RemoteControl` | Executes remote commands and publishes the status |

---

## Known Limitations

- The Wi-Fi connection is made at boot and can take up to 30 seconds. The buttons work only after that.
- MQTT does not reconnect after the connection is lost. Local operation continues to work.
