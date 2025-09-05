# ESP32-C3 LED Matrix Control

This project uses an **ESP32-C3** and a **MAX7219-based LED Matrix display** (via the MD_Parola library) to display scrolling text.  
A **captive portal-style web interface** is hosted on the ESP32, allowing users to update the displayed text and scrolling speed from any Wi-Fi device.

---

## ✨ Features
- 📶 ESP32-C3 runs in **Access Point mode** (no external Wi-Fi needed).
- 🖥️ Captive web interface with a **modern responsive UI**.
- 🔤 Update the **display text** dynamically.
- ⏱️ Control the **scrolling speed** (10–500 ms).
- 🎨 Stylish, mobile-friendly HTML/CSS with subtle animations.
- ⚡ Real-time updates via `/setText` endpoint.

---

## 🛠️ Hardware Requirements
- **ESP32-C3 board**
- **MAX7219 8x8 LED Matrix** (FC16 module)
- Jumper wires / breadboard

---

## 🔌 Wiring
| MAX7219 Pin | ESP32-C3 Pin |
|-------------|--------------|
| DIN         | GPIO 0       |
| CLK         | GPIO 1       |
| CS          | GPIO 2       |
| VCC         | 3.3V / 5V    |
| GND         | GND          |

---

## 📦 Libraries Used
Install these libraries in Arduino IDE (via Library Manager or GitHub):
- [`WiFi.h`](https://github.com/espressif/arduino-esp32)
- [`ESPAsyncWebServer`](https://github.com/me-no-dev/ESPAsyncWebServer)
- [`MD_MAX72XX`](https://github.com/MajicDesigns/MD_MAX72XX)
- [`MD_Parola`](https://github.com/MajicDesigns/MD_Parola)
- [`SPI.h`](https://www.arduino.cc/en/reference/SPI)

---

## 👨‍💻 Author
Created by **Roy Cuadra**  
For educational and IoT prototype development 🚀

---
