# Medication_Dispenser_and_Tracker


An IoT-based automatic pill dispenser designed to improve medication adherence through NFC-based patient verification, timed dispensing, and real-time caregiver notifications via Slack.

---

## Problem Statement

Medication non-adherence is a major challenge in healthcare and often leads to severe health complications, increased hospitalizations, and higher medical costs. Existing solutions such as pill organizers and reminder systems do not adequately prevent missed doses, overdosing, or incorrect medication intake.

This project aims to address these issues by providing an automated, secure, and connected medication dispensing system.

---

## Project Overview

The Automatic Medicine Dispenser is an intelligent IoT device that:

- Dispenses medication only at scheduled times
- Verifies patient identity using NFC
- Prevents unauthorized or accidental dispensing
- Detects missed doses and notifies caregivers
- Allows caregivers to remotely dispense medication via Slack
- Logs all medication events on a backend server

---

## Key Features

- NFC-based patient authentication
- Time-based medication scheduling
- Automatic pill dispensing using servo motors
- Missed dose detection with real-time alerts
- Remote dispensing through Slack commands
- Visual and audio feedback using LEDs, buzzer, and OLED
- Backend logging of medication events

---

## System Architecture

The system consists of three major components:

1. Embedded hardware and firmware (ESP8266)
2. Backend server (Flask + Python)
3. Slack-based caregiver interface

**Data flow:**

NFC Card  
→ ESP8266 Controller  
→ Servo Motors / OLED / LEDs / Buzzer  
→ Flask Backend Server  
→ Slack Notifications

---

## Use Cases

### Use Case 1: NFC-based Medication Dispensing

- The dispenser alerts the patient at the scheduled time
- The patient scans their NFC card within a defined time window
- The system verifies identity and dispenses the correct medication
- The event is logged and a confirmation is sent to the caregiver via Slack

### Use Case 2: Missed Dose Detection

- The dispenser alerts the patient at the scheduled time
- The patient does not scan the NFC card within the allowed time
- The system triggers a missed dose alert
- The caregiver receives a real-time Slack notification

### Use Case 3: Remote Dispensing via Slack

- A caregiver sends a dispense command via Slack
- The backend verifies the patient ID
- The dispenser releases the medication
- The event is logged and confirmed via Slack

---
<img width="509" height="554" alt="Screenshot 2026-01-03 at 12 04 39 PM" src="https://github.com/user-attachments/assets/e7f8b42a-3921-41e8-9c12-6e54791d0099" />


## Hardware Components

- ESP8266 (Wemos D1 Mini Lite)
- PN532 NFC Reader
- Two micro servo motors (slot rotation and dispensing gate)
- RGB LED
- Buzzer
- OLED display
- Resistors (230 ohms)
- Breadboard and jumper wires

---

## Software Stack

### Firmware

- Arduino-based firmware for ESP8266
- NFC communication using PN532 library
- Servo control using PWM
- Non-blocking timing logic using `millis()`
- HTTP communication with backend server

### Backend

- Python
- Flask web framework
- Slack API (commands and notifications)
- Database for logging medication events

---
<img width="362" height="374" alt="Screenshot 2026-01-03 at 12 10 28 PM" src="https://github.com/user-attachments/assets/c649a75c-e281-42b3-8107-9b1e0eda8da1" />
<img width="722" height="403" alt="Screenshot 2026-01-03 at 12 09 58 PM" src="https://github.com/user-attachments/assets/e98e8663-79d5-4fee-8498-8e2575d10ea9" />

## Implementation Notes

- Non-blocking logic is used to avoid ESP8266 delays
- Passive NFC detection prevents blocking execution
- Servo motors are controlled via predefined rotation angles for each patient slot
- Slack communication is handled by a backend server to improve scalability

---

## Evaluation Summary

A small case study with five participants was conducted.

- All participants successfully completed NFC-based dispensing
- Missed dose detection worked as expected
- Remote dispensing via Slack functioned reliably
- Users found the system intuitive and easy to use
- Caregivers valued real-time notifications and remote control

---

## Limitations

- Limited number of patients supported due to slot-based design
- Servo motor rotation limited to approximately 180 degrees
- HTTPS certificate validation disabled for prototype Slack integration

---

## Future Work

- Support for a larger number of patients
- Stepper motor or carousel-based dispensing mechanism
- Secure HTTPS communication
- Dosage verification using weight sensors
- Mobile application integration
- Enhanced authentication and security

---


