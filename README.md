# ESP32 Cold Chain Tracker ️📍

A production-grade IoT telemetry system for real-time monitoring of temperature-sensitive assets during transport. Built for the pharmaceutical cold chain industry with enterprise-level error handling and graceful degradation.

## 🚀 Features

- **Real-Time Temperature Monitoring** - DS18B20 waterproof sensor with automatic error detection
- **GPS Location Tracking** - Live latitude/longitude coordinates for fleet tracking
- **Battery Monitoring** - Voltage divider circuit for remote battery level reporting
- **Cloud Connectivity** - Automatic WiFi reconnection and HTTP POST to webhooks
- **Graceful Degradation** - Continues sending GPS data even if temperature sensor fails
- **Compile-Time Feature Flags** - Enable/disable features without code bloat
- **Production-Ready Error Handling** - Validates all sensor readings before transmission

## 📦 Hardware Requirements

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP32 DevKitC (WROOM-32) | 1 | Main controller |
| DS18B20 Waterproof Temp Sensor | 1 | Temperature monitoring |
| GPS Module (SIM28/NEO-6M) | 1 | Location tracking |
| 100kΩ Resistor | 2 | Voltage divider for battery |
| 4.7kΩ Resistor | 1 | Pull-up for DS18B20 |
| Breadboard + Jumper Wires | 1 set | Prototyping |
| 18650 Battery + TP4056 Charger | 1 | Power (optional for testing) |


## 🔌 Wiring Diagram

### Temperature Sensor (DS18B20)
```text
ESP32          DS18B20
-----          -------
3.3V    -----> VCC
GND     -----> GND
GPIO4   -----> DATA
              |
4.7kΩ Resistor between DATA and VCC (pull-up)
```

### GPS Module
```text
ESP32          GPS
-----          ---
GPIO16  -----> TX (GPS transmit)
GPIO17  -----> RX (GPS receive)
3.3V    -----> VCC
GND     -----> GND
```

### Battery Monitoring (Voltage Divider)
```text
Battery (+) -----> 100kΩ Resistor -----> GPIO34 (ADC)
                                     |
                                100kΩ Resistor
                                     |
Battery (-) ------------------------+-----> GND

Note: This divides battery voltage by 2 to safely read 4.2V Li-ion on 3.3V ESP32 pin
```

## 💻 Software Setup

### Prerequisites
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE Extension](https://platformio.org/platformio-ide)

### Installation

1. **Clone the repository:**
```bash
git clone https://github.com/yourusername/esp32-cold-chain-tracker.git
cd esp32-cold-chain-tracker
```

2. **Open in VS Code:**
```bash
code .
```

3. **PlatformIO will automatically:**
   - Download ESP32 framework
   - Install required libraries (OneWire, DallasTemperature, TinyGPSPlus)
   - Build the project

4. **Build and Upload:**
   - Click the **✓ (Build)** button in the PlatformIO toolbar
   - Click the **→ (Upload)** button to flash to ESP32
   - Open **Serial Monitor** (baud rate: 115200) to view logs

## ⚙️ Configuration

### WiFi Credentials
Edit `src/main.cpp`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### Webhook URL
Get your free endpoint at [webhook.site](https://webhook.site):
```cpp
const char* webhookUrl = "https://webhook.site/YOUR_UNIQUE_ID";
```

### Feature Flags
Enable/disable features at compile-time to save Flash memory:
```cpp
#define ENABLE_BATTERY_MONITOR true  // Set to false if not using battery
```

##  JSON Payload Example

Your device will send data every 3 seconds:

```json
{
  "temperature": 25.37,
  "unit": "C",
  "lat": 24.860735,
  "lon": 67.001137,
  "battery_pct": 87.5
}
```

**Error Handling:**
- If temperature sensor disconnected: `"temperature": null`
- If GPS has no fix yet: `"lat": null, "lon": null`
- If battery monitoring disabled: `"battery_pct": null`

## 🛠️ How It Works

### 1. **Temperature Reading**
- Uses DallasTemperature library for 12-bit precision (±0.5°C)
- Automatically detects disconnected sensors (`DEVICE_DISCONNECTED_C = -127`)
- 750ms conversion delay for accurate readings

### 2. **GPS Parsing**
- Uses TinyGPSPlus library to decode NMEA sentences
- Reads from Hardware Serial 2 (GPIO 16/17)
- Validates location fix before sending

### 3. **WiFi Management**
- Auto-reconnects if connection drops (max 20 attempts)
- Uses non-blocking checks to prevent system hangs
- Reports IP address on successful connection

### 4. **Battery Monitoring**
- Reads analog voltage on GPIO 34 (input-only pin)
- Voltage divider scales 4.2V battery to 2.1V (safe for ESP32)
- Maps 3.0V-4.2V range to 0%-100% battery percentage

### 5. **Data Transmission**
- HTTP POST to webhook every 3 seconds
- JSON format for easy integration with dashboards
- Gracefully handles partial sensor failures

## 📈 Memory Usage

Typical build output:
```text
RAM:   [==        ]  14.4% (used 47KB from 320KB)
Flash: [=======   ]  70.8% (used 928KB from 4MB)
```

**Flash breakdown:**
- Your code: ~3KB
- Libraries: ~15KB
- ESP32 Core: ~150KB
- WiFi/HTTP Stack: ~600KB
- Free space: ~1.2MB (for future features)

##  Future Enhancements

- [ ] **OLED Display** - Show live data on device
- [ ] **OTA Updates** - Update firmware over WiFi
- [ ] **Deep Sleep Mode** - Extend battery life to weeks
- [ ] **MQTT Protocol** - Replace HTTP for lower power
- [ ] **Multiple Sensors** - Monitor up to 8 temperature points
- [ ] **Cellular Backup** - Switch to 4G when WiFi unavailable
- [ ] **Web Configuration Portal** - Configure WiFi without hardcoding

## 🏭 Real-World Applications

- **Pharmaceutical Transport** - Vaccine cold chain monitoring
- **Food & Beverage** - Frozen food logistics
- **Blood Banks** - Temperature-controlled storage
- **Laboratory Equipment** - Freezer monitoring
- **Agriculture** - Greenhouse climate control

##  License

MIT License - feel free to use for commercial or personal projects.

## 🤝 Contributing

This is a production-ready MVP. Contributions welcome for:
- Bug fixes
- Performance optimizations
- New sensor support
- Documentation improvements

## 📞 Support

For questions or consultation on commercial deployment, open an issue on GitHub.

---

**Built with ❤️ for the Industrial IoT revolution**

*ESP32 + PlatformIO + DallasTemperature + TinyGPSPlus*