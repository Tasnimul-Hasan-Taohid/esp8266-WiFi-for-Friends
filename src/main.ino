/**
 * ESP8266 WiFi Repeater with Open Guest Network
 * 
 * Features:
 * - Connects to existing WiFi network (STA mode)
 * - Creates open guest WiFi network (AP mode)
 * - Internet sharing with bandwidth limiting
 * - Web dashboard for status and configuration
 * - Captive portal for easy setup
 * 
 * Hardware: NodeMCU ESP8266
 * Framework: Arduino (via PlatformIO)
 */

#include "config.h"
#include "wifi_manager.h"
#include "captive_portal.h"
#include "web_server.h"
#include "nat_router.h"

// Global objects
WiFiManager* wifi_mgr = nullptr;
CaptivePortal* captive = nullptr;
WebServer* web_srv = nullptr;
NATRouter* nat_router = nullptr;

// Setup function - runs once at startup
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(DEBUG_SERIAL_SPEED);
  delay(100);
  
  // Print startup banner
  Serial.println("\n\n");
  Serial.println("═══════════════════════════════════════════════════════");
  Serial.println("  ESP8266 WiFi Repeater v1.0");
  Serial.println("  Free Guest Network Manager");
  Serial.println("═══════════════════════════════════════════════════════");
  Serial.print("  Build: ");
  Serial.println(__DATE__ " " __TIME__);
  Serial.print("  Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.println("═══════════════════════════════════════════════════════\n");
  
  // Create and initialize WiFi manager
  wifi_mgr = new WiFiManager();
  wifi_mgr->init();
  
  // Create and initialize captive portal
  captive = new CaptivePortal(wifi_mgr);
  captive->init();
  
  // Create and initialize web server
  web_srv = new WebServer(wifi_mgr);
  web_srv->init();
  
  // Create and initialize NAT router
  nat_router = new NATRouter(wifi_mgr, web_srv);
  nat_router->init();
  
  Serial.println("[Setup] All modules initialized successfully");
}

// Loop function - runs continuously
void loop() {
  // Update all modules in sequence
  
  // Update WiFi manager (handles STA/AP connection logic)
  if (wifi_mgr) {
    wifi_mgr->update();
  }
  
  // Update captive portal (process DNS requests)
  if (captive) {
    captive->update();
  }
  
  // Update web server (handle HTTP requests)
  if (web_srv) {
    web_srv->update();
  }
  
  // Update NAT router (forward packets, apply bandwidth limiting)
  if (nat_router) {
    nat_router->update();
  }
  
  // Yield control to prevent watchdog timeout
  yield();
}

// Debug function to print system info
void printSystemInfo() {
  if (!ENABLE_DEBUG) return;
  
  Serial.println("\n--- System Information ---");
  Serial.print("WiFi SSID: ");
  Serial.println(wifi_mgr->getSTASSID());
  Serial.print("STA IP: ");
  Serial.println(wifi_mgr->getSTAIP());
  Serial.print("AP IP: ");
  Serial.println(wifi_mgr->getAPIP());
  Serial.print("Connected Clients: ");
  Serial.println(wifi_mgr->getConnectedClients());
  Serial.print("Signal Strength: ");
  Serial.print(wifi_mgr->getSTASignal());
  Serial.println(" dBm");
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("Packets Forwarded: ");
  Serial.println(nat_router->getPacketsForwarded());
  Serial.println();
}
