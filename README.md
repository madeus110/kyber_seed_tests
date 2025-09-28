# Kyber Seed Tests ESP32 Implementation

This project implements seed expansion and timing measurements of the CRYSTALS-Kyber post-quantum cryptographic scheme for the ESP32-S3 platform.  
It focuses on low-level experimentation with the raw seed generation and expansion step, measuring execution times to evaluate potential timing side channels.  
Kyber512-only implementation optimized for memory-constrained ESP32-S3 hardware.

---

## Project Structure

The repository assumes the following structure:

esp/

├── esp-idf/ # ESP-IDF toolchain (not included)

└── kyber-seed-tests-esp32/ # This project

Only `kyber-seed-tests-esp32/` is included in this repository.  
You must install ESP-IDF manually by following the official Espressif installation guide:

👉 https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/

---

## Features

- Written in standard C for ESP32-S3 target.  
- Implements Kyber seed generation and expansion using SHAKE256.  
- Runs **10,000 full iterations** to stress-test the algorithm.  
- Each iteration records:  
  - Execution time in microseconds (µs).  
  - The resulting expanded seed (hexadecimal).  
- Results are written to a text file on the ESP32 (via basic file I/O).  
- Designed to investigate potential timing variability (e.g., **295 µs vs 305 µs**) and confirm stability.  

---

## Building and Flashing

Make sure you are in the `esp/kyber-seed-tests-esp32` directory and that your ESP-IDF environment is correctly sourced.

```bash
. ../esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
