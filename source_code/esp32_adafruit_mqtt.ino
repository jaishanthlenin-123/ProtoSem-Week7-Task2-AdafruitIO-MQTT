/*
 * Task 2: Adafruit IO Cloud Dashboard & MQTT Communication
 * Protocol: MQTT over TCP (Port 1883)
 * Feed: [AIO_USERNAME]/feeds/led-control
 * Hardware: ESP32 DevKit, LED on GPIO 2 (330 ohm resistor to GND)
 */

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Network Credentials
#define WLAN_SSID       "YOUR_WIFI_SSID"
#define WLAN_PASS       "YOUR_WIFI_PASSWORD"

// Adafruit IO Broker Configuration
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "YOUR_ADAFRUIT_IO_USERNAME"
#define AIO_KEY         "YOUR_ADAFRUIT_IO_KEY"

// Hardware Pin Definition (Matches Pin Table and Schematic)
const int ledPin = 2;

// Instantiate WiFi and MQTT Clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Define MQTT Subscription Feed for Remote Control
Adafruit_MQTT_Subscribe ledFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led-control");

// Helper function to establish and maintain MQTT connection
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("\nStarting Adafruit IO MQTT Client...");
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected! IP: " + WiFi.localIP().toString());

  // Register subscription listener
  mqtt.subscribe(&ledFeed);
  Serial.println("Subscribed to MQTT Feed: " + String(AIO_USERNAME) + "/feeds/led-control");
}

void loop() {
  // Ensure MQTT connection stays alive
  MQTT_connect();

  // Poll for incoming subscription packets (timeout = 2000ms)
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(2000))) {
    if (subscription == &ledFeed) {
      char *payload = (char *)ledFeed.lastread;
      Serial.print("[MQTT RECEIVED] Feed: led-control | Value: ");
      Serial.println(payload);

      if (strcmp(payload, "ON") == 0 || strcmp(payload, "1") == 0) {
        digitalWrite(ledPin, HIGH);
        Serial.println("[ACTION] GPIO 2 Driven HIGH (LED ON)");
      } else if (strcmp(payload, "OFF") == 0 || strcmp(payload, "0") == 0) {
        digitalWrite(ledPin, LOW);
        Serial.println("[ACTION] GPIO 2 Driven LOW (LED OFF)");
      }
    }
  }

  // MQTT keep-alive ping (required every few seconds to prevent broker timeout)
  if (!mqtt.ping()) {
    mqtt.disconnect();
  }
}

// Reconnection routine with retry counter
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Connecting to Adafruit IO MQTT Broker... ");
  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      Serial.println("Failed to connect after retries. Yielding to main loop.");
      return;
    }
  }
  Serial.println("MQTT Broker Connected!");
}