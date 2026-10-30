# Task 2 Technical Implementation Report
## Adafruit IO Dashboard & MQTT Integration

### Executive Summary
This project establishes bi-directional cloud telemetry and actuation between an ESP32 edge microcontroller and the Adafruit IO platform over the lightweight Message Queuing Telemetry Transport (MQTT) protocol on Port 1883.

### MQTT Protocol Specifications
- **Broker Host:** `io.adafruit.com`
- **Port:** `1883` (Standard unencrypted MQTT)
- **Keep-Alive Interval:** 60 seconds
- **Subscription Feed:** `jaishanthlenin-123/feeds/lamp-control`
- **Publishing Feed:** `jaishanthlenin-123/feeds/sensor-telemetry`
- **QoS Level:** QoS 0 (At most once delivery)
