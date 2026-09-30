# ESP32 Indoor Air Quality & Climate Monitor

An ESP32-based environmental telemetry node that monitors **ambient temperature**, **relative humidity**, and **compensated indoor air quality (VOCs / smoke / combustible gases)** using a **DHT11** and **MP-4 MOS gas sensor**.

## Features
- **32x Multisampled ADC Filtering:** Uses ESP32 factory-calibrated `analogReadMilliVolts()` with 32-sample averaging to eliminate Wi-Fi/ADC noise.
- **Auto-Baseline Calibration ($R_0$):** Automatically samples clean air on startup to establish the sensor baseline resistance.
- **Real-Time Thermal & Humidity Compensation:** Dynamically adjusts the MP-4 resistance ratio ($R_s / R_0$) using live DHT11 temperature and humidity readings to prevent humidity-induced false alarms.

## Hardware & Pinout (30-Pin ESP32 DevKit V1)

| Sensor Module | Sensor Pin | ESP32 Pin | Physical Position (USB at Bottom) | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **DHT11** | `S` (Left) | `GPIO 4` (`D4`) | Right side, 5th from bottom | 1-Wire digital data |
| **DHT11** | `VCC` (Middle) | `3V3` | Right side, 1st from bottom | 3.3V logic power |
| **DHT11** | `-` (Right) | `GND` | Right side, 2nd from bottom | Common ground |
| **MP-4 Gas** | `VCC` | `VIN` (`5V`) | Left side, 1st from bottom | 5V USB power for internal heater |
| **MP-4 Gas** | `GND` | `GND` | Left side, 2nd from bottom | Common ground |
| **MP-4 Gas** | `AO` | `GPIO 34` (`D34`) | Left side, 4th from top | `ADC1_CH6` analog input |
| **MP-4 Gas** | `DO` | `GPIO 27` (`D27`) | Left side, 6th from bottom | Optional digital threshold |

## Version History
- **`v1.0`** — Baseline serial telemetry with `R0` auto-calibration and DHT11 temperature/humidity drift compensation.
- **`v2.0` (Planned)** — integration of LCD display and Live Wi-Fi Web Dashboard