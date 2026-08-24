#include <stdint.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>

// 1. WiFi Credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// 2. Your Webhook URL
const char* webhookUrl = "https://webhook.site/563f87b8-eabd-4b9e-8c73-9adf84bcdfab";

// 3. Sensor Setup
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// 4. GPS Setup (Using Hardware Serial 2)
#define GPS_RX 16
#define GPS_TX 17
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

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
  
  // Initialize GPS Serial (9600 baud is standard for GPS)
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  // Connect to Internet
  connectToInternet();

  // Initialize Sensor
  sensors.begin();
  Serial.println("System Started! Sensors ready.");
}

void loop() {
  // 1. Read Temperature
  sensors.requestTemperatures();
  delay(750);
  float tempC = sensors.getTempCByIndex(0);
  
  // 2. Read GPS Data
  // Feed characters from the GPS serial port into the TinyGPSPlus parser
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 3. Send to Webhook (if Internet is connected)
  if (isInternetAvailable()) {
    HTTPClient http;
    http.begin(webhookUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Build JSON payload
    String jsonData = "{";
    if (tempC != DEVICE_DISCONNECTED_C) {
      jsonData += "\"temperature\": " + String(tempC, 2) + ",";
    } else {
      jsonData += "\"temperature\": null,";
    }
    jsonData += "\"unit\": \"C\",";
    
    // Add GPS data if a valid location fix is available
    if (gps.location.isValid()) {
      jsonData += "\"lat\": " + String(gps.location.lat(), 6) + ",";
      jsonData += "\"lon\": " + String(gps.location.lng(), 6);
    } else {
      jsonData += "\"lat\": null, \"lon\": null";
    }
    
    jsonData += "}";
    
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

  // Wait 3 seconds before next reading (gives GPS time to parse)
  delay(3000);
}