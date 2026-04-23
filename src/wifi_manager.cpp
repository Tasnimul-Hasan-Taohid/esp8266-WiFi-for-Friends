#include "wifi_manager.h"
#include "config.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>

WiFiManager::WiFiManager()
    : sta_connected(false), 
      ap_running(false), 
      config_mode(false),
      led_state(false),
      led_blink_interval(LED_BLINK_SLOW),
      last_led_toggle(0),
      sta_connection_timeout(0),
      dhcp_server(nullptr) {
  memset(&wifi_config, 0, sizeof(wifi_config));
  loadConfig();
}

void WiFiManager::init() {
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off (active low)
  
  // Initialize reset button
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  
  // Set WiFi mode to dual (STA + AP)
  WiFi.mode(WIFI_AP_STA);
  
  // Start AP mode immediately (for configuration)
  startAP();
  
  // Attempt to connect to saved WiFi
  if (strlen(wifi_config.ssid) > 0) {
    config_mode = false;
    connectToWiFi();
  } else {
    config_mode = true;
    if (ENABLE_DEBUG) {
      Serial.println("[WiFiMgr] No saved WiFi - entering config mode");
    }
  }
}

void WiFiManager::update() {
  // Check reset button
  checkResetButton();
  
  // Update LED status
  updateLED();
  
  // Monitor STA connection
  if (!sta_connected && !config_mode) {
    if (millis() - sta_connection_timeout > STA_RECONNECT_INTERVAL) {
      connectToWiFi();
      sta_connection_timeout = millis();
    }
  }
  
  // Check if STA connection was lost
  if (sta_connected && WiFi.status() != WL_CONNECTED) {
    sta_connected = false;
    led_blink_interval = LED_BLINK_SLOW;
    if (ENABLE_DEBUG) {
      Serial.println("[WiFiMgr] STA connection lost, attempting reconnection...");
    }
  }
  
  // Handle configuration portal timeout
  if (config_mode && (millis() > CONFIG_PORTAL_TIMEOUT)) {
    if (sta_connected) {
      config_mode = false;
      if (ENABLE_DEBUG) {
        Serial.println("[WiFiMgr] Config portal timeout, switching to normal mode");
      }
    }
  }
}

void WiFiManager::startAP() {
  if (ap_running) return;
  
  // Configure AP mode
  WiFi.softAPConfig(
    IPAddress(AP_IP),
    IPAddress(AP_GATEWAY),
    IPAddress(AP_SUBNET)
  );
  
  // Start AP
  bool result = WiFi.softAP(
    wifi_config.ap_ssid,
    AP_PASSWORD,
    AP_CHANNEL,
    AP_HIDDEN,
    AP_MAX_CONNECTIONS
  );
  
  if (result) {
    ap_running = true;
    
    if (ENABLE_DEBUG) {
      Serial.print("[WiFiMgr] AP started: ");
      Serial.print(wifi_config.ap_ssid);
      Serial.print(" @ ");
      Serial.println(WiFi.softAPIP());
    }
  } else {
    if (ENABLE_DEBUG) {
      Serial.println("[WiFiMgr] Failed to start AP");
    }
  }
}

void WiFiManager::stopAP() {
  if (!ap_running) return;
  
  WiFi.softAPdisconnect(true);
  ap_running = false;
  
  if (ENABLE_DEBUG) {
    Serial.println("[WiFiMgr] AP stopped");
  }
}

void WiFiManager::connectToWiFi() {
  if (strlen(wifi_config.ssid) == 0) {
    config_mode = true;
    return;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  
  if (ENABLE_DEBUG) {
    Serial.print("[WiFiMgr] Connecting to: ");
    Serial.println(wifi_config.ssid);
  }
  
  WiFi.begin(wifi_config.ssid, wifi_config.password);
  
  sta_connection_timeout = millis();
  led_blink_interval = LED_BLINK_SLOW;
}

bool WiFiManager::isSTAConnected() const {
  return (WiFi.status() == WL_CONNECTED);
}

bool WiFiManager::isAPRunning() const {
  return ap_running;
}

bool WiFiManager::isConfigMode() const {
  return config_mode;
}

IPAddress WiFiManager::getSTAIP() const {
  return WiFi.localIP();
}

IPAddress WiFiManager::getAPIP() const {
  return WiFi.softAPIP();
}

int WiFiManager::getSTASignal() const {
  return WiFi.RSSI();
}

uint8_t WiFiManager::getConnectedClients() const {
  return WiFi.softAPgetStationNum();
}

const char* WiFiManager::getSTASSID() const {
  return WiFi.SSID().c_str();
}

void WiFiManager::setWiFiConfig(const char* ssid, const char* password, const char* ap_ssid) {
  memset(wifi_config.ssid, 0, sizeof(wifi_config.ssid));
  memset(wifi_config.password, 0, sizeof(wifi_config.password));
  memset(wifi_config.ap_ssid, 0, sizeof(wifi_config.ap_ssid));
  
  strncpy(wifi_config.ssid, ssid, sizeof(wifi_config.ssid) - 1);
  strncpy(wifi_config.password, password, sizeof(wifi_config.password) - 1);
  strncpy(wifi_config.ap_ssid, ap_ssid, sizeof(wifi_config.ap_ssid) - 1);
  
  saveConfig();
  config_mode = false;
  connectToWiFi();
  
  if (ENABLE_DEBUG) {
    Serial.println("[WiFiMgr] WiFi config updated and saved");
  }
}

void WiFiManager::resetConfig() {
  memset(&wifi_config, 0, sizeof(wifi_config));
  strncpy(wifi_config.ap_ssid, DEFAULT_AP_SSID, sizeof(wifi_config.ap_ssid) - 1);
  
  saveConfig();
  config_mode = true;
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect(true);  // Disconnect and turn off STA
  }
  
  sta_connected = false;
  
  if (ENABLE_DEBUG) {
    Serial.println("[WiFiMgr] WiFi config reset to defaults");
  }
}

void WiFiManager::saveConfig() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(CONFIG_START, wifi_config);
  EEPROM.commit();
  EEPROM.end();
}

void WiFiManager::loadConfig() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(CONFIG_START, wifi_config);
  EEPROM.end();
  
  // Ensure null-terminated strings
  wifi_config.ssid[sizeof(wifi_config.ssid) - 1] = 0;
  wifi_config.password[sizeof(wifi_config.password) - 1] = 0;
  wifi_config.ap_ssid[sizeof(wifi_config.ap_ssid) - 1] = 0;
  
  // Set defaults if not configured
  if (strlen(wifi_config.ap_ssid) == 0) {
    strncpy(wifi_config.ap_ssid, DEFAULT_AP_SSID, sizeof(wifi_config.ap_ssid) - 1);
  }
}

void WiFiManager::checkResetButton() {
  static uint32_t button_press_time = 0;
  static bool button_pressed = false;
  
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    if (!button_pressed) {
      button_pressed = true;
      button_press_time = millis();
    } else if ((millis() - button_press_time) > 5000) {
      // Long press detected (5 seconds)
      resetConfig();
      button_pressed = false;
      button_press_time = 0;
    }
  } else {
    button_pressed = false;
  }
}

void WiFiManager::updateLED() {
  uint32_t now = millis();
  
  if (isSTAConnected()) {
    // Solid on when connected
    digitalWrite(LED_PIN, LOW);
  } else if (led_blink_interval > 0) {
    // Blinking when not connected
    if ((now - last_led_toggle) > led_blink_interval) {
      led_state = !led_state;
      digitalWrite(LED_PIN, led_state ? HIGH : LOW);
      last_led_toggle = now;
    }
  }
}

WiFiManager::~WiFiManager() {
}
