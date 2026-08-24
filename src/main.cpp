#include <stdint.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// 1. WiFi Credentials (Use your home WiFi, or any dummy text for Wokwi simulation)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// 2. Your Webhook URL
const char* webhookUrl = "https://webhook.site/563f87b8-eabd-4b9e-8c73-9adf84bcdfab";

// 3. Sensor Setup
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool isInternetAvailable() {
  return WiFi.status() == WL_CONNECTED;
}

void connectToInternet() {
  Serial.print("Connecting to Internet ");
  uint8_t attempts = 0;
  WiFi.begin(ssid, password);
  while (!isInternetAvailable() && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (isInternetAvailable()) {
    Serial.println("\nInternet Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to Internet.");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Connect to Internet
  connectToInternet();

  // Initialize Sensor
  sensors.begin();
  Serial.println("System Started! Sensor ready.");
}

void loop() {
  // 1. Read Temperature
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  
  // Check if reading is valid (DS18B20 returns -127 if disconnected)
  if (tempC != -127.0) {
    // 2. Send to Webhook (if Internet is connected)
    if (isInternetAvailable()) {
      HTTPClient http;
      http.begin(webhookUrl);
      http.addHeader("Content-Type", "application/json");
      
      // Create JSON payload
      String jsonData = "{\"temperature\": " + String(tempC, 2) + ", \"unit\": \"C\"}";
      
      // Send POST request
      int httpResponseCode = http.POST(jsonData);
      
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error sending data: ");
        Serial.println(httpResponseCode);
      }
      
      http.end(); // Free resources
    } else {
      Serial.println("Internet not available, reconnecting...");
      connectToInternet();
    }
  } else {
    Serial.println("Error: Could not read temperature. Check wiring.");
  }

  // Wait 1 seconds before next reading
  delay(1000);
}