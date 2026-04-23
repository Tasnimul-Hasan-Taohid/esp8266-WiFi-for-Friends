# ESP8266 WiFi Repeater with Open Guest Network

A production-ready WiFi repeater device built on the ESP8266 (NodeMCU) microcontroller that bridges an existing WiFi network to a secondary open guest network.

## 🌟 Features

### Core Functionality
- **Dual WiFi Mode**: Simultaneously connect to an existing WiFi network (Station mode) and create your own open WiFi network (Access Point mode)
- **Internet Sharing**: Automatically share internet from the main WiFi to guest clients using NAT routing
- **Multi-Client Support**: Support for up to 5+ simultaneous guest connections
- **Persistent Configuration**: WiFi credentials saved to EEPROM (survives power cycles)
- **Auto-Reconnect**: Automatically reconnects to main WiFi if connection drops

### Network Management
- **Configurable Guest SSID**: Set custom name for your guest network (default: "Free_Guest_WiFi")
- **Open Network**: No password required—guests can connect immediately
- **DHCP Server**: Automatic IP address assignment to connected devices
- **Bandwidth Limiting**: Optional "Slow Mode" that adds artificial delays to limit bandwidth

### User Interface
- **Web Dashboard**: Beautiful, responsive web interface showing:
  - Primary network connection status
  - Guest network statistics (connected clients, IP addresses)
  - Real-time signal strength monitoring
  - Configuration management
- **Captive Portal**: First-time visitors are automatically redirected to setup page
- **Admin Route**: Hidden `/admin` endpoint for debugging and system info

### Hardware Integration
- **LED Status Indicator**:
  - Solid = Connected to main WiFi
  - Blinking = Attempting to connect
  - Off = Disconnected
- **Reset Button**: Long-press (5 seconds) to reset all WiFi settings and return to config mode

## 📋 Hardware Requirements

### Components
- **Microcontroller**: NodeMCU ESP8266 (or compatible ESP8266 board)
- **USB Power**: 5V micro-USB cable for programming and power
- **Optional**: Push button (for reset), LED indicator (status light)

### Pinout
```
Reset Button  → GPIO13 (D7)
Status LED    → GPIO15 (D8)
GND           → GND
3.3V          → 3.3V
```

### Power Consumption
- Average: ~80-150mA @ 5V
- Peak: ~200mA @ 5V
- **Recommended PSU**: 5V / 1A minimum

## 🚀 Quick Start

### Installation with PlatformIO

```bash
# Install PlatformIO
pip install platformio

# Clone or download this repository
cd esp8266-wifi-repeater

# Connect NodeMCU via USB

# Build and upload
platformio run -t upload

# Monitor output
platformio device monitor
```

### Installation with Arduino IDE

1. Install ESP8266 board package:
   - Preferences → Additional Board Manager URLs
   - Add: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Board Manager → Search "esp8266" → Install
2. Select Board: Tools → Board → NodeMCU 1.0
3. Select Port: Tools → Port → Your USB port
4. Open `src/main.ino`
5. Click Upload

### Initial Setup

**First Boot (No WiFi Saved):**
1. Device creates temporary AP: `"Free_Guest_WiFi"`
2. Connect with phone/laptop
3. Open browser → `http://192.168.4.1`
4. Click **⚙️ Configure**
5. Enter:
   - Primary Network SSID
   - Password
   - Guest Network Name (optional)
6. Click **Save** → Reboot and connect

## 🔧 Configuration

Edit `src/config.h` to customize:
```cpp
#define DEFAULT_AP_SSID "Free_Guest_WiFi"    // Guest network name
#define AP_MAX_CONNECTIONS 5                  // Max clients
#define PACKET_DELAY_MS 10                   // Bandwidth delay
#define PACKET_DELAY_SLOW_MS 50              // Slow mode delay
```

## 📊 Web Dashboard

Access at: `http://192.168.4.1` (when connected to guest network)

**Endpoints:**
- `GET /api/status` - System status as JSON
- `POST /api/config` - Update WiFi configuration
- `POST /api/toggle-slow` - Toggle bandwidth limiting
- `GET /admin` - Admin debug panel

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| LED not lighting | Check USB power, verify correct GPIO pin (D8) |
| Can't connect to guest network | Ensure open network, check power, try "forget network" |
| Can't access dashboard | Verify WiFi connection, try `192.168.4.1` directly |
| Main WiFi won't connect | Check SSID/password, ensure 2.4GHz, check signal |
| Serial monitor errors | Check baud rate is 115200, verify USB cable |

## 📈 Performance

- **Max Clients**: 5-7 simultaneous
- **Throughput (Normal)**: ~10-15 Mbps
- **Throughput (Slow Mode)**: ~1-3 Mbps
- **Latency**: 5-20ms (normal), 50-100ms (slow)
- **Range**: ~30m open space, ~10m with walls

## 📝 File Structure

```
esp8266-wifi-repeater/
├── src/
│   ├── main.ino              # Main sketch
│   ├── config.h              # Configuration
│   ├── wifi_manager.cpp/.h   # WiFi management
│   ├── captive_portal.cpp/.h # DNS captive portal
│   ├── web_server.cpp/.h     # HTTP server & UI
│   └── nat_router.cpp/.h     # Packet routing
├── platformio.ini            # Build config
└── README.md                 # This file
```

## 📄 License

MIT License - See LICENSE file

## 🤝 Support

For issues or questions, please open a GitHub issue.

---

**Enjoy your ESP8266 WiFi Repeater!** 🎉
