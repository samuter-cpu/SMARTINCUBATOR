# AGENTS.md – Jules AI Instructions

## ?? Project Purpose
Automate egg incubation and hatching using EK-TM4C1294XL, EWARM, and QPC framework. Control temperature, humidity, CO2, motor rotation, ventilation, and UI with reactive active objects and sensor events.

## ?? Key Files
- `Source/main.c` – System startup, AO initialization
- `Source/bsp.c` – Peripheral setup, ISRs, onStartup/onIdle
- `Source/TemperatureControl.c` – Active object for thermal control
- `Source/humidityControl.c` – Humidity control logic
- `Source/Co2Control.c` – Air quality control and ventilation trigger
- `Source/MotorControl.c` – Rack motor logic, directional timers
- `Source/LcdDisplay.c` – Dual I²C display handler
- `Source/HatchMode.c` – Mode switch to hatching logic
- `Include/IncubatorHatcher.h` – Signals, prototypes, event structures
-and others 

## ?? Sensors & Actuators
- `DHT11` – Internal + external temp/humidity
- `SCD30` or `MH-Z19C` – CO2 + O2 sensors (suggested)
- `FS300A` or `SDP3x` – Airflow sensors (suggested)
- `28BYJ-48 Stepper` – Rack motor
- `PWM Heater`, `Valve-controlled Cooler`, `Ultrasonic Humidifier`
- `Active Buzzer`, `I²C LCDs`, `Ventilation Fans`

## ?? Tasks for Jules AI

### 1. Documentation
- Summarize state machines in `temperature_ctrl.c`, `humidity_ctrl.c`, `co2_ctrl.c`
- Describe AO interactions and event signals in `incubator_hatcher.h`
- Map mode transitions (Incubation ? Hatch) from `hatch_mode.c`

### 2. Visualization
- Generate flowcharts for control logic and AO coordination in `/Docs/statecharts.md`
- Create `/Docs/system_diagram.md` showing sensor-actuator-event relationships

### 3. Refactoring & Optimization
- Ensure consistent AO naming and modularity
- Organize control logic for better readability (split PI from On/Off sections)
- Suggest abstraction layers for sensor publishing

## ?? Safety Events
- Trigger `AlarmControl` AO for temperature/humidity deviation > ±1.5°C/10% RH for > 3 minutes
- Activate buzzer and LCD blinking until user override or sensor recovery

## ?? Restrictions
- Do NOT modify `.ewp`, `.ewd`, or external libraries (TI / QPC core folders)
- Keep IAR compatibility intact

