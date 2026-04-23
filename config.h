#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ESP8266 WiFi Repeater Configuration
// ============================================================================

// Hardware Pins
#define LED_PIN D8                    // GPIO15 - Status LED
#define RESET_BUTTON_PIN D7           // GPIO13 - Reset WiFi settings button

// WiFi Configuration
#define DEFAULT_AP_SSID "Free_Guest_WiFi"
#define AP_PASSWORD ""                // Empty = open network
#define AP_CHANNEL 6
#define AP_MAX_CONNECTIONS 5
#define AP_HIDDEN false

// IP Configuration for AP
#define AP_IP 192, 168, 4, 1
#define AP_GATEWAY 192, 168, 4, 1
#define AP_SUBNET 255, 255, 255, 0
#define AP_DNS 8, 8, 8, 8

// Web Server
#define WEB_SERVER_PORT 80
#define CONFIG_PORTAL_TIMEOUT 300000  // 5 minutes in ms

// Network Parameters
#define DHCP_LEASE_TIME 3600          // 1 hour
#define STA_RECONNECT_INTERVAL 10000  // 10 seconds
#define STA_MAX_RETRY 20
#define RSSI_CHECK_INTERVAL 30000     // 30 seconds

// Bandwidth Limiting
#define DEFAULT_SLOW_MODE false
#define PACKET_DELAY_MS 10            // Artificial delay between packet forwards
#define PACKET_DELAY_SLOW_MS 50       // Slow mode delay

// Storage
#define CONFIG_START 32
#define CONFIG_SIZE 512
#define EEPROM_SIZE 4096

// Debug Settings
#define DEBUG_SERIAL_SPEED 115200
#define ENABLE_DEBUG true
#define ENABLE_TELEMETRY true

// LED Indicators
#define LED_BLINK_SLOW 1000           // ms - connecting
#define LED_BLINK_FAST 200            // ms - error
#define LED_SOLID 0                   // ms - connected

#endif // CONFIG_H
