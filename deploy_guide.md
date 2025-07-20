# ?? Deployment Guide for SmartIncubatorProject

This guide walks through flashing, configuring, and running the embedded firmware on the EK-TM4C1294XL using IAR Embedded Workbench (EWARM).

---

## ?? Toolchain Requirements

- IDE: IAR Embedded Workbench for ARM (EWARM), v9.30+ recommended
- MCU: EK-TM4C1294XL
- Framework: QPC (QP/C), including `qp`, `qp_port`, and `qps`
- Libraries:
  - TI TivaWare Peripheral Drivers
  - I2C LCD Driver
  - DHT11 Protocol Handler

---

## ?? Build Configuration

1. Open `.eww` workspace in IAR EWARM.
2. Exclude unused blinky examples:
   - Right-click Blinky files in project tree ? "Remove from build"
3. Confirm include paths:
   - `Include/` and `QPC/` directories must be in the compiler's search paths.
4. Select correct device:
   - Project ? Options ? General Options ? Device: `TM4C1294NCPDT`

---

## ?? Flashing Firmware

1. Connect EK-TM4C1294XL via USB.
2. Select “Download and Debug” in IAR EWARM.
3. Monitor device COM port via terminal (e.g., PuTTY) at 115200 baud.
4. Watch for startup messages like:
   - `"System Init Complete"`
   - `"Incubation Mode Started"`

---

## ?? Runtime Setup

### Mode Initialization
- System boots in Incubation Mode
- Internal temperature set to 37.1°C, humidity to 56%
- LCD 1 displays temperature and humidity
- LCD 2 displays countdown timer and CO2/O2 levels

### Switch Usage
- Switch 1: Increase set temperature/humidity
- Switch 2: Decrease set temperature/humidity

### Egg Rack Motor
- Begins 180° cycle every 45 minutes

---

## ?? Manual Interventions

- If buzzer activates and LCD blinks continuously:
  - Confirm environmental deviation via LCD
  - Adjust parameters manually or override via onboard switches
  - Watch for `"ALARM CLEARED"` feedback

---

## ?? Troubleshooting

| Issue                      | Cause / Fix                                      |
|---------------------------|--------------------------------------------------|
| LCD not displaying         | Check I²C wiring and address                     |
| No sensor data             | Ensure DHT11/CO2 sensor connections are correct |
| Motor not rotating         | Check 28BYJ-48 driver logic and power supply    |
| No UART log output         | Confirm terminal settings and USB enumeration   |

---

## ?? Final Note

After successful incubation (18 days), Hatch Mode activates automatically. Ensure environmental shifts are visible on both LCD screens and confirm vent motor activation.

