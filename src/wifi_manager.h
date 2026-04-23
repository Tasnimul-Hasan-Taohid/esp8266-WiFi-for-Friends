#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdint.h>
#include <IPAddress.h>

// Forward declarations
class DHCPServer;

struct WiFiConfig {
  char ssid[32];
  char password[64];
  char ap_ssid[32];
};

class WiFiManager {
public:
  WiFiManager();
  ~WiFiManager();
  
  // Initialization and main loop
  void init();
  void update();
  
  // AP control
  void startAP();
  void stopAP();
  
  // STA control
  void connectToWiFi();
  
  // Status queries
  bool isSTAConnected() const;
  bool isAPRunning() const;
  bool isConfigMode() const;
  
  IPAddress getSTAIP() const;
  IPAddress getAPIP() const;
  int getSTASignal() const;
  uint8_t getConnectedClients() const;
  const char* getSTASSID() const;
  
  // Configuration
  void setWiFiConfig(const char* ssid, const char* password, const char* ap_ssid);
  void resetConfig();
  
private:
  WiFiConfig wifi_config;
  
  bool sta_connected;
  bool ap_running;
  bool config_mode;
  bool led_state;
  
  uint32_t led_blink_interval;
  uint32_t last_led_toggle;
  uint32_t sta_connection_timeout;
  
  DHCPServer* dhcp_server;
  
  // Internal methods
  void saveConfig();
  void loadConfig();
  void checkResetButton();
  void updateLED();
};

#endif // WIFI_MANAGER_H
