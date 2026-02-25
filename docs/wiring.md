# AEGIS-C3 — Hardware Wiring Guide

Hardware: **Seeed Studio XIAO ESP32-C3**

---

## Component List

| Component | Notes |
|---|---|
| Seeed XIAO ESP32-C3 | Main MCU |
| 1.3" OLED (128×64, SH1106) | Status display over I2C |
| LED (any colour) | Alert indicator |
| 220Ω resistor | Current limiter for LED |
| Breadboard + jumper wires | |
| USB-C cable | Power + flashing |

---

## XIAO ESP32-C3 Pinout Reference

```
                         USB-C
                   ┌──────────────┐
              GND  │ GND      5V  │  5V
  ⚠️ GPIO 2  D0  │ D0       D10 │  GPIO 10   (MOSI)
     GPIO 3  D1  │ D1        D9 │  GPIO 9 ⚠️ (MISO / BOOT btn)
     GPIO 4  D2  │ D2        D8 │  GPIO 8 ⚠️ (SCK / strapping)
     GPIO 5  D3  │ D3        D7 │  GPIO 20   (UART RX)
     GPIO 6  D4  │ D4 (SDA)  D6 │  GPIO 21   (UART TX)
     GPIO 7  D5  │ D5 (SCL) 3V3 │  3.3V
              GND └──────────────┘
```

### ⚠️ Strapping Pins — do not connect external hardware here

| Pin | GPIO | Risk |
|---|---|---|
| D0 | GPIO2 | Pulled LOW at boot → can affect boot mode selection |
| D8 | GPIO8 | Must be HIGH during download boot (flashing) |
| D9 | GPIO9 | Connected to onboard BOOT button; LOW on reset = bootloader |

### Other Reserved Pins

| GPIO | Reason to avoid |
|---|---|
| GPIO 12–17 | Internal SPI flash — not broken out, do not use |
| GPIO 18–19 | USB-JTAG peripheral — used internally |
| GPIO 20–21 | UART0 (Serial monitor RX/TX) — avoid for other use |

### ADC Note
D3 (GPIO5) uses **ADC2**, which can produce false or inoperative readings. For any
analog sensing, use **D0/D1/D2 (ADC1)** — but note D0 is a strapping pin, so prefer
**D1 (GPIO3)** or **D2 (GPIO4)** for analog inputs.

---

## OLED Display Wiring (SH1106, 1.3")

| OLED Pin | XIAO Pin | Notes |
|---|---|---|
| VCC | 3V3 | 3.3V only — do not use 5V |
| GND | GND | |
| SDA | D4 (GPIO6) | Hardware I2C data |
| SCL | D5 (GPIO7) | Hardware I2C clock |

```
XIAO ESP32-C3          1.3" OLED
┌──────────┐           ┌──────────┐
│  3V3 ────┼───────────┼─ VCC     │
│  GND ────┼───────────┼─ GND     │
│  D4  ────┼───────────┼─ SDA     │
│  D5  ────┼───────────┼─ SCL     │
└──────────┘           └──────────┘
```

**Troubleshooting the OLED:**
If the display is blank or shows garbage, the module may use an SSD1306 controller
instead of SH1106. Open `firmware/src/alerts.cpp` and swap the constructor:

```cpp
// For SH1106 (1.3" — default in this project):
static U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// For SSD1306 (0.96" — swap to this if display is blank):
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
```

---

## LED Wiring

The LED is wired to **D1 (GPIO3)** — a clean general-purpose GPIO with no strapping or
boot-mode role.

> **Why not D0 (GPIO2)?** GPIO2 is a strapping pin. The LED+resistor circuit pulls it
> LOW through GND. At power-on, before firmware runs, this LOW level is sampled by the
> chip and can cause it to enter an unexpected boot mode, potentially locking out
> programming. Always keep strapping pins unloaded at boot.

| Connection | Detail |
|---|---|
| LED anode (+) | D1 (GPIO3) via 220Ω resistor |
| LED cathode (−) | GND |

```
XIAO D1 (GPIO3)
      │
     [ 220Ω ]
      │
     [LED +]
     [LED −]
      │
     GND
```

---

## Full Wiring Diagram (ASCII)

```
                      XIAO ESP32-C3
                   ┌───────────────────┐
               GND─┤GND           3V3  ├─── OLED VCC
    GPIO3 ─[220Ω]──┤D1  (LED)     D5  ├─── OLED SCL
                   │              D4  ├─── OLED SDA
                   │              GND ├─── OLED GND
                   └───────────────────┘
```

---

## Software Pin Configuration

Pins are defined in `firmware/src/config.h`:

```cpp
#define PIN_SDA   6   // D4 — OLED SDA (GPIO6)
#define PIN_SCL   7   // D5 — OLED SCL (GPIO7)
#define PIN_LED   3   // D1 — alert LED (GPIO3) — NOT a strapping pin
```

---

## Flashing the Firmware

1. Install [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
2. Open the `firmware/` folder.
3. Edit `firmware/src/config.h` — set your Wi-Fi SSID, password, and backend IP.
4. Connect the XIAO ESP32-C3 via USB-C.
5. Run:

```bash
cd firmware
pio run --target upload
pio device monitor --baud 115200
```

On successful boot, the Serial monitor will show:

```
=============================
       AEGIS-C3 BOOT
=============================
[Alerts] OLED + LED initialised
[WiFi] Connected: 192.168.1.XX
[Honeypot] Listening on port 80
[AEGIS-C3] System ready
```

**If upload fails or the board becomes unresponsive:**
Hold the **BOOT button** (connected to D9/GPIO9), plug in USB-C while holding it, then
release. This forces bootloader mode regardless of strapping pin state.

---

## Detection Thresholds (configurable in config.h)

| Parameter | Default | Meaning |
|---|---|---|
| `SCAN_THRESHOLD` | 5 | Unique path hits within 10s → scan event |
| `BRUTE_THRESHOLD` | 3 | POST /admin attempts within 30s → brute force event |
| `SCAN_WINDOW_MS` | 10000 | Scan detection window (ms) |
| `BRUTE_WINDOW_MS` | 30000 | Brute force detection window (ms) |
