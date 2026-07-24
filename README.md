# 🌱 ESP32 Smart Plant Monitoring System

An IoT-based smart plant monitoring and automatic watering system built using an ESP32.

The system monitors environmental and soil conditions in real time, displays the measurements locally and through a web dashboard, and automatically waters the plant when the soil becomes dry.

---

## 📋 Features

- 🌡 Temperature and humidity monitoring using DHT11
- 🌱 Soil moisture monitoring
- 💧 Automatic watering based on soil moisture level
- 📺 Real-time OLED display
- 🌐 Wi-Fi web dashboard
- 🔘 Manual watering through the web interface
- 🔌 Relay-controlled water pump
- 🛠 Custom-built DC water pump
- 🖥 Custom PCB designed in KiCad

---

## ⚙️ How It Works

The ESP32 continuously reads data from the temperature, humidity, and soil moisture sensors.

When the soil moisture drops below the defined threshold, the ESP32 activates the relay, which powers the water pump.

Sensor readings and system status are displayed on both the OLED screen and the Wi-Fi web dashboard.

**System flow:**

`Sensors → ESP32 → Control Logic → Relay → Water Pump`

`ESP32 → OLED Display + Web Dashboard`

---

## 🛠 Hardware

- ESP32 DevKit V1
- DHT11 Temperature & Humidity Sensor
- Soil Moisture Sensor
- SSD1306 OLED Display
- Relay Module
- Custom-built DC Water Pump
- Custom PCB

---

## 💻 Software

- Arduino IDE
- KiCad 10
- ESP32 Web Server

---

## 🔧 Working Prototype

The complete system was first assembled and tested on a breadboard before moving to the custom PCB design.

![Working Prototype](images/prototype.jpg)

---

## 📺 OLED Display

The OLED provides real-time information including temperature, air humidity, soil moisture, pump status, and plant watering status.

![OLED Display](images/oled-display.jpg)

---

## 💧 Custom-built DC Water Pump

Instead of using a ready-made water pump, a small DC water pump was built from scratch for the project.

The pump uses a DC motor, a custom-made impeller, and a handmade housing with inlet and outlet connections.

It is controlled by the ESP32 through a relay and can be activated automatically when the soil moisture level is low.

![Custom-built DC Water Pump](images/Custom-built%20DC%20Water%20Pump1.jpg)

![Custom-built DC Water Pump](images/Custom-built%20DC%20Water%20Pump2.jpg)

---

## 📐 Schematic

The complete circuit schematic was designed in KiCad.

![Schematic](images/Schematic.png)

---

## 🖥 PCB Layout

A custom PCB was designed to integrate the ESP32 and the system's sensors and peripherals.

![PCB Layout](images/PCB%20Layout.png)

---

## 📦 3D PCB View

3D visualization of the custom PCB design.

![3D PCB View](images/3D%20PCB%20View.png)

---

## 🌐 Web Dashboard

The ESP32 hosts a local web interface that allows real-time monitoring of the plant and manual activation of the watering system.

![Smart Plant Web Dashboard](images/Web%20Dashboard.png)

---

## 📁 Repository Structure

- `firmware/` → ESP32 source code
- `pcb/` → KiCad schematic and PCB design files
- `gerber/` → PCB manufacturing files
- `images/` → Project images and screenshots

---

## 🚀 Current Status

- ✅ Breadboard prototype completed
- ✅ Sensors successfully integrated
- ✅ OLED interface completed
- ✅ Wi-Fi web dashboard completed
- ✅ Automatic watering system working
- ✅ Custom water pump built and tested
- ✅ Firmware completed
- ✅ PCB designed in KiCad
- ✅ Gerber manufacturing files generated
- ⏳ Physical PCB manufacturing and assembly coming soon

---

## 👨‍💻 Author

**Antreas Petrou**

Electrical Engineering Student  
Aristotle University of Thessaloniki

---

## 📄 License

This project is licensed under the MIT License.
