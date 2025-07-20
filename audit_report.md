# SMARTINCUBATOR System-Level Audit Report

This report provides a comprehensive analysis of the SMARTINCUBATOR project, focusing on active object coordination, memory allocation, module completeness, and system consistency.

### 1. Active Object Coordination

The project uses the QP-C framework to implement a system of active objects that communicate with each other using events. The coordination between the active objects is generally well-defined, but there are some areas that need improvement.

**Key Findings:**

*   **Event-Driven Architecture:** The system is built on a solid event-driven architecture. The use of signals and events for communication between AOs is consistent and well-structured.
*   **Centralized Event Definitions:** The `IncubatorHatcher.h` file serves as a central repository for all signal and event definitions, which is good practice.
*   **Incomplete State Machines:** The `HatchMode` active object has an incomplete state machine. The `CANDLE_DAY_SIG` transitions to a non-existent state, and the `CandleDaytimeEvt` is not handled.
*   **Incorrect Event Casting:** The `LcdDisplay` active object incorrectly casts events, which can lead to undefined behavior.
*   **Missing Header Files:** The `TemperatureControl.h` and `HumidityControl.h` header files are missing from the project.

**Recommendations:**

*   **Complete the `HatchMode` state machine:** Implement the `HatchMode_Candle` state and handle the `CANDLE_DAY_SIG` and `CandleDaytimeEvt` events.
*   **Correct the event casting in `LcdDisplay.c`:** Use the correct syntax for casting events to their specific types.
*   **Create the missing header files:** Create `TemperatureControl.h` and `HumidityControl.h` and declare the public interface for these AOs.

### 2. Event Pool & Memory Allocation

The system uses a single, global event pool managed by the QF framework. Each active object has its own event queue.

**Key Findings:**

*   **Small Event Queue Sizes:** The event queues for all active objects are fixed at a size of 10. This is too small and could lead to queue overflow, especially for AOs that handle multiple event types like `AlarmControl` and `LcdDisplay`.
*   **Undefined Global Event Pool Size:** The size of the global event pool is not defined in the project files. This makes it impossible to determine if the pool is large enough to handle the application's needs.

**Recommendations:**

*   **Increase event queue sizes:** Increase the event queue sizes for all active objects to at least 20.
*   **Define the global event pool size:** The size of the global event pool should be explicitly defined and should be large enough to accommodate the worst-case scenario of event usage.
*   **Consider using separate event pools:** For a more robust system, consider using separate event pools for different AOs or for events of different sizes.

### 3. Module Completeness & Functional Audit

The audit revealed several incomplete modules and functions.

**Key Findings:**

*   **Missing BSP Functions:** Many of the hardware-dependent functions in `bsp.c` are not implemented.
*   **Incomplete Fan Control Logic:** The fan control logic in `Co2Control.c` is a simple on/off control. It does not adjust the fan speed based on the CO2 level.
*   **Incorrect PI Control Implementation:** The PI control logic in `TemperatureControl.c` and `HumidityControl.c` is not correctly implemented. The output of the PI controller is not used to set the power of the heater, cooler, or humidifier.
*   **Stale Data in Structs:** The `current_temp` and `current_humidity` members of the `TemperatureControl` and `HumidityControl` structs are not updated with the latest sensor readings.
*   **Undefined Signal:** The `ALARM_Off_SIG` signal is used in `AlarmControl.c` but is not defined anywhere in the project.

**Recommendations:**

*   **Implement all BSP functions:** Implement all the missing hardware-dependent functions in `bsp.c`.
*   **Improve the fan control logic:** Implement a more sophisticated fan control algorithm that adjusts the fan speed based on the CO2 level.
*   **Correct the PI control implementation:** Use the output of the PI controller to set the power of the actuators.
*   **Update the structs with sensor readings:** Update the `current_temp` and `current_humidity` members of the structs with the latest sensor readings.
*   **Define the `ALARM_OFF_SIG` signal:** Define the `ALARM_OFF_SIG` signal in `IncubatorHatcher.h`.

### 4. System Consistency & Redundancy Checks

The audit identified several areas of redundancy in the code.

**Key Findings:**

*   **Redundant Alarm Logic:** The alarm logic is repeated in `TemperatureControl.c`, `HumidityControl.c`, and `Co2Control.c`.
*   **Redundant Sensor Reading Logic:** The sensor reading logic is spread across multiple files.

**Recommendations:**

*   **Create a generic alarm AO:** Create a new active object that is responsible for handling the alarm logic for all the sensors.
*   **Create a sensor AO:** Create a new active object that is responsible for reading all the sensors and publishing the data to the control AOs.
*   **Refactor PI control:** The PI control logic could be further abstracted by creating a generic PI controller AO.

This concludes the system-level audit report. By addressing the issues and implementing the recommendations outlined in this report, the SMARTINCUBATOR project can be made more robust, reliable, and maintainable.
