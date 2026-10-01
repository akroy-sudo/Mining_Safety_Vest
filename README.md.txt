# Smart Mining Safety Vest

An IoT-based Smart Mining Safety Vest designed to improve miner safety through real-time environmental monitoring, hazardous gas detection, fall detection, and emergency SOS alerts.

The system uses an ESP32 microcontroller, multiple sensors, MQTT communication, and a Node-RED dashboard to monitor safety parameters and provide alerts.

## 🚀 Features

* **Environmental Monitoring:** Monitors temperature and humidity using the DHT11 sensor.
* **Gas Detection:** Uses MQ-135 and MQ-7 sensors to monitor potentially hazardous gases.
* **Fall Detection:** Uses the MPU6050 accelerometer and gyroscope to detect abnormal movement and potential falls.
* **Emergency SOS:** Includes a dedicated SOS button for emergency alerts.
* **Audible Alerts:** A buzzer provides alerts for SOS activation, falls, and gas-related conditions.
* **IoT Monitoring:** Sends sensor data to an MQTT broker for remote monitoring.
* **Live Dashboard:** Displays sensor readings and safety alerts using Node-RED.

## 🛠️ Hardware Components

| Component   | Purpose                             |
| ----------- | ----------------------------------- |
| ESP32       | Main microcontroller                |
| DHT11       | Temperature and humidity monitoring |
| MQ-135      | Air quality and gas monitoring      |
| MQ-7        | Carbon monoxide (CO) detection      |
| MPU6050     | Motion and fall detection           |
| Push Button | Emergency SOS                       |
| Buzzer      | Audible safety alerts               |

## 🔌 Pin Configuration

| Component            | ESP32 Pin |
| -------------------- | --------- |
| DHT11 Data           | GPIO 32   |
| MQ-135 Analog Output | GPIO 34   |
| MQ-7 Analog Output   | GPIO 35   |
| SOS Button           | GPIO 33   |
| Buzzer               | GPIO 25   |
| MPU6050 SDA          | GPIO 21   |
| MPU6050 SCL          | GPIO 22   |

**Note:** Pin assignments are based on the current project configuration. Verify them against your actual circuit before deployment.

## ⚙️ System Architecture

1. Sensors collect environmental and motion data.
2. The ESP32 processes sensor readings.
3. Configured thresholds are used to identify potential safety hazards.
4. The buzzer provides local alerts for detected conditions.
5. Sensor data is transmitted through MQTT.
6. Node-RED displays readings and safety statuses on a dashboard.

## 📡 Communication

* **Wi-Fi:** Connects the ESP32 to the network.
* **MQTT:** Transfers sensor readings and status information.
* **Node-RED:** Visualizes incoming data through dashboard gauges and indicators.

The current MQTT topic used by the project is:

`vest/data`

## 💻 Software Requirements

* Arduino IDE
* ESP32 Board Package
* ESP32-compatible sensor libraries
* MQTT client library, such as PubSubClient
* Node-RED
* MQTT broker

## 🔔 Alert Conditions

The system is designed to provide alerts for:

* Potential falls based on motion and orientation.
* Gas readings exceeding configured thresholds.
* Manual SOS activation.

The buzzer follows a priority system in which SOS alerts take precedence over fall alerts, followed by gas alerts.

## 🔧 Setup Instructions

1. Install the Arduino IDE.
2. Install the ESP32 board package.
3. Install the required sensor and MQTT libraries.
4. Connect the sensors according to the pin configuration.
5. Open the ESP32 firmware file in Arduino IDE.
6. Configure your Wi-Fi credentials and MQTT broker settings.
7. Upload the firmware to the ESP32.
8. Configure Node-RED to receive and display MQTT messages.
9. Power on the system and verify sensor readings and alerts.

## 📊 Dashboard

The Node-RED dashboard can be used to visualize:

* Temperature and humidity
* MQ-135 and MQ-7 readings
* SOS status
* Fall detection status
* Gas alert status
* Other configured safety indicators

## 🔮 Future Scope

* Integration of an ESP32-CAM for visual monitoring.
* GPS-based location tracking for emergency situations.
* Cloud-based data storage and historical analysis.
* Improved gas calibration and fall-detection accuracy.
* Integration with mine-wide safety monitoring systems.

## ⚠️ Disclaimer

This project is a prototype developed for educational and research purposes. Sensor readings and detection thresholds require proper calibration and validation. This system is not a certified mining safety device and should not replace approved industrial safety equipment.

## 👨‍💻 Project

**Smart Mining Safety Vest — IoT-Based Miner Safety Monitoring System**

Developed using ESP32, environmental sensors, motion sensing, MQTT, and Node-RED.
