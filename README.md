# IoT-Based Electric Leakage Detection and Alert System

## 1. Project Overview & Aim
This project designs and implements an **Internet of Things (IoT)** embedded security framework deployed on an **ESP32** to monitor alternating current (AC) lines for early electrical fault identification. Operating on electromagnetic induction principles, the system captures real-time leakage currents without direct electrical contact to mitigate electrical fire risks, terminal short circuits, and severe operational shocks.

## 2. Problem Statement & Motivation
Traditional safety architectures (like standard ELCBs) operate as passive tripping mechanisms without logging capability. They frequently fail to catch micro-leakage anomalies below their rigid tripping boundaries, and manual periodic insulation testing is highly inefficient. This system introduces continuous, 24/7 non-invasive monitoring to provide real-time local and cloud-based alerts before major system damage occurs.

## 3. System Hardware Architecture & Technical Stack
The system abstracts data across physical hardware components and digital cloud frameworks seamlessly:

*   **Microcontroller Core:** NodeMCU ESP32 (utilizing 12-bit ADC mapping resolution).
*   **Sensor Node:** SCT-013 Split-Core Current Transformer (Non-invasive clip setup).
*   **Signal Conditioning:** Burden Resistor network matched with an analog voltage smoothing circuit.
*   **Local Interface:** 16x2 I2C LCD Screen paired with an active high-decibel warning buzzer array.
*   **Cloud Platform API:** ThingSpeak IoT Infrastructure for live time-series analytical monitoring.
*   **Development Ecosystem:** Arduino IDE programmed in Embedded C/C++.

## 4. Analytical Working Logic & Calibration Pipeline
1.  **Electromagnetic Induction Sensing:** The SCT-013 transformer clips directly over the active live AC mains wire, outputting a secondary high-frequency current proportional to the primary field flux.
2.  **Burden Conversion & Offset Calibration:** The burden circuit drops this current into a measurable voltage wave. The ESP32 collects **600 raw samples** over its analog input pin to extract a stable median tracking offset.
3.  **True RMS Computation:** A mathematical root-mean-square loop processes the analog sample arrays, subtracting the calculated offset to extract true, real-time AC root values.
4.  **Threshold Evaluation & Alert Matrix:** The edge-computed loop tracks values against a defined safety ceiling (**5.0 Amperes**):
    *   **Under 5.0A (STATUS: SAFE):** System logs clean telemetry data.
    *   **Exceeds 5.0A (STATUS: DANGER!):** The physical local buzzer triggers instantly, and the 16x2 LCD shifts from a nominal status print directly into an active warning display.

## 5. IoT Telemetry Logging & Results
The firmware initializes a local 2.4GHz Wi-Fi gateway handshake using a structured HTTP GET request string to pump fields straight to the **ThingSpeak cloud architecture** every 20 seconds. This enables continuous remote monitoring, real-time wave evaluation, and historic fault trace lookups across distinct execution phases.
