# 🌱 ESP32 Smart Plant Monitoring System

An IoT project that monitors plant conditions and automatically controls watering using an ESP32 microcontroller.

## 📋 Features

- 🌡 Temperature and humidity monitoring (DHT11)
- 🌱 Soil moisture monitoring
- 💧 Automatic watering using a relay and water pump
- 📺 OLED display
- 🌐 Wi-Fi web interface
- 🔌 Custom PCB designed in KiCad

---

## 🛠 Hardware

- ESP32 DevKit V1
- DHT11 Sensor
- Soil Moisture Sensor
- SSD1306 OLED Display
- Relay Module
- Custom-built DC Water Pump

---

## 💻 Software

- Arduino IDE
- KiCad 10

---

## 📁 Repository Structure

- `firmware/` → ESP32 firmware
- `pcb/` → KiCad project files
- `gerber/` → PCB manufacturing files
- `images/` → Project images

---

## 📷 Schematic

![Schematic](images/Schematic.png)

---

## 🖥️ PCB Layout

![PCB Layout](images/PCB%20Layout.png)

---

## 📦 3D PCB View

![3D PCB View](images/3D%20PCB%20View.png)

---

## 🌐 Web Dashboard

![Smart Plant Web Dashboard](images/Web%20Dashboard.png)

---

## 🔧 Working Prototype

The system was first developed and tested on a breadboard before designing the custom PCB.

![Working Prototype](images/prototype.jpg)

### 📺 OLED Display

Real-time sensor readings and watering status are displayed locally on the OLED screen.

![OLED Display](images/oled-display.jpg)


## 💧 Custom-built DC Water Pump

The water pump was designed and built from scratch using a small DC motor, a custom impeller, and a handmade housing.

The pump is controlled by the ESP32 through a relay and is used for automatic watering when the soil moisture level is low.

### Pump Construction

![Custom-built DC Water Pump](images/Custom-built%20DC%20Water%20Pump1.jpg)

![Custom-built DC Water Pump](images/Custom-built%20DC%20Water%20Pump2.jpg)

## 🚀 Current Status

- ✅ Prototype completed
- ✅ Firmware completed
- ✅ Custom PCB designed
- ✅ Gerber files generated
- ⏳ Physical PCB assembly coming soon

---

## 👨‍💻 Author

**Antreas Petrou**

Electrical Engineering Student  
Aristotle University of Thessaloniki

---

## 📄 License

This project is licensed under the MIT License.
