# Physics Lab Final Project: Air Quality Monitoring System

This repository contains code for a dual ESP32-based air quality monitoring system, featuring real-time data collection and visualization.

## Overview

This project consists of two ESP32 microcontrollers communicating via ESP-NOW protocol:
- **Air Sensor**: Collects environmental data (PM1, PM2.5, PM10, CO2, temperature, humidity)
- **Air Monitor**: Displays data on TFT screen with LED indicators and buzzer alerts

## Hardware Components

### Air Sensor Module
- ESP32 microcontroller
- PMS3003 particulate matter sensor
- TGS4161 CO2 sensor
- DHT22 temperature & humidity sensor

### Air Monitor Module
- ESP32 microcontroller
- TFT display (320x240)
- WS2811 LED strip (8 LEDs)
- Buzzer for alerts

## Features

- Real-time air quality monitoring
- Wireless data transmission using ESP-NOW
- Visual feedback with color-coded LEDs
- WiFi server for data logging
- Python script for data collection to text files

## Software Requirements

- PlatformIO
- Python 3.x
- Libraries: LVGL, TFT_eSPI, FastLED, ESP-NOW, DHT

## Usage

1. Upload firmware to both ESP32 modules using PlatformIO
2. Power on the Air Sensor module to start collecting data
3. Air Monitor displays real-time readings on screen
4. Connect to the ESP's wifi and run `wifi.py` to log data to text files (pm2.5.txt, co2.txt, temp.txt)

## Project Structure

```
Air Sensor/    - Sensor module firmware
Air Monitor/   - Display module firmware with UI
wifi.py        - Python data logging script
```

