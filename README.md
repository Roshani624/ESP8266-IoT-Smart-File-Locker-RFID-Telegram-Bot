#  ESP8266 IoT Smart File Locker  

### RFID Authentication • Telegram Bot Alerts • Web Dashboard

<br>

##  Overview

Traditional locker systems lack authentication, monitoring, and automation.
This project implements a **real-time IoT-enabled smart locker system** using **ESP8266 (NodeMCU)** and **Arduino Nano**, integrating RFID-based authentication with automated control and live monitoring.

The system enables **secure document access, event tracking, and remote monitoring** through a web dashboard and Telegram Bot notifications.

<br>

##  Key Features

*  **RFID-Based Authentication** – UID-based access control for authorized users
*  **Dual RFID Architecture** – Independent modules for user authentication and file detection
*  **Automated Servo Locking** – PWM-controlled servo ensures automatic lock/unlock
*  **Real-Time Web Dashboard** – Displays locker state, file presence, and logs
*  **Telegram Bot Integration** – Instant push alerts using Telegram Bot API
*  **Event Logging System** – Timestamp-based access tracking

<br>

##  System Architecture

The system follows a **distributed embedded + IoT architecture**:

* **NodeMCU (ESP8266)**
  → Handles Wi-Fi communication and hosts the web server
* **Arduino Nano**
  → Processes RFID input and controls actuator logic
* **RFID Reader 1**
  → User authentication (UID verification)
* **RFID Reader 2**
  → File presence detection
* **Servo Motor**
  → Executes locking mechanism using PWM control
* **Web Dashboard (HTTP Server)**
  → Displays real-time system state and logs

<br>

##  Workflow

1. RFID tag is scanned
2. UID is verified against authorized database
3. If valid → servo motor unlocks locker
4. File insertion/removal detected via second RFID
5. NodeMCU updates dashboard via Wi-Fi
6. Telegram Bot sends event notification
7. Locker auto-locks after operation

<br>

##  Tech Stack

### Hardware

* NodeMCU ESP8266 (Wi-Fi-enabled microcontroller)
* Arduino Nano (ATmega328P)
* RFID Modules (MFRC522)
* Servo Motor (PWM controlled)
* LEDs (status indication)

### Software

* Embedded C (Arduino IDE)
* ESP8266 Web Server (HTTP protocol)
* HTML/CSS (Dashboard UI)
* Telegram Bot API (Real-time alerts)

<br>

##  Results

* ✅ Accurate UID-based user authentication
* ✅ Reliable dual RFID detection mechanism
* ✅ Real-time dashboard updates via Wi-Fi
* ✅ Low-latency Telegram notifications
* ✅ Stable performance across multiple test cases

<br>

## 📂 Repository Structure

```
ESP8266-IoT-Smart-File-Locker-RFID-Telegram-Bot/
│
├── report/
├── presentation/
├── poster/
├── code/
├── images/
└── README.md
```

<br>

##  Key Highlights

* Embedded + IoT system integration
* Real-time communication using ESP8266 Wi-Fi stack
* Event-driven architecture with alert system
* Low-cost and scalable design for institutional deployment

<br>

##  Future Scope

*  Biometric authentication (Fingerprint / Face Recognition)
*  Cloud integration for remote access & storage
*  Mobile app interface
*  AI-based anomaly detection for security

<br>

##  Contributors

* Challa Abhinaya Sree
* Polamarasetty Vineela
* Roshani S
* Nandhakishore K

<br>

##  Conclusion

This project demonstrates the integration of **Embedded Systems + IoT + Real-Time Communication** to build a secure, automated, and intelligent document locker system.

<br>

## ⭐ Support

If you found this project useful, consider giving it a ⭐ on GitHub!
