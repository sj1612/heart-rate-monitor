# Heart Rate and Blood Oxygen (SpO₂) Monitoring Device

##  Overview

This project presents a non-invasive optical biosensing device designed to measure heart rate and blood oxygen saturation (SpO₂) using fingertip photoplethysmography (PPG). The system combines analog signal conditioning with embedded digital processing for accurate physiological measurement.

---

## Objectives

* Acquire PPG signals using optical sensing
* Condition weak biosignals using low-noise analog circuitry
* Digitize and process signals using an embedded microcontroller
* Compute heart rate and SpO₂ using standard physiological formulas

---

## System Architecture

### Optical Sensing

* IR LED transmitter and photodiode receiver
* Fingertip-based reflective PPG signal acquisition

### Analog Front End

* Transimpedance amplifier for current-to-voltage conversion
* Multi-stage op-amp amplification
* Active bandpass filtering to isolate pulsatile components

### Embedded Processing

* ESP32 microcontroller
* High-resolution ADC sampling
* Digital signal processing and parameter computation

---


## Tech Stack

* Analog Electronics (Op-amps, Active Filters)
* ESP32 Microcontroller
* Embedded C / Arduino Framework
* LTspice (circuit simulation)

---

## Output

* Heart Rate (BPM)
* Blood Oxygen Saturation (SpO₂ %)

---

##  License

For academic and educational use.
