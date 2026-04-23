#include "nat_router.h"
#include "config.h"
#include <ESP8266WiFi.h>

NATRouter::NATRouter(WiFiManager* wm, WebServer* ws)
    : wifi_manager(wm),
      web_server(ws),
      packets_forwarded(0),
      last_packet_time(0),
      current_delay_ms(PACKET_DELAY_MS) {
}

void NATRouter::init() {
  if (ENABLE_DEBUG) {
    Serial.println("[NATRouter] NAT router initialized");
  }
}

void NATRouter::update() {
  if (!wifi_manager->isSTAConnected() || !wifi_manager->isAPRunning()) {
    return;
  }
  
  // Update packet delay based on slow mode
  if (web_server->isSlowModeEnabled()) {
    current_delay_ms = PACKET_DELAY_SLOW_MS;
  } else {
    current_delay_ms = PACKET_DELAY_MS;
  }
  
  // Implement artificial delay between packet processing
  uint32_t now = millis();
  if ((now - last_packet_time) > current_delay_ms) {
    last_packet_time = now;
    packets_forwarded++;
  }
  
  // Log periodic stats
  static uint32_t last_stats_time = 0;
  if ((now - last_stats_time) > 60000) {  // Every 60 seconds
    logStats();
    last_stats_time = now;
  }
}

void NATRouter::logStats() {
  if (!ENABLE_TELEMETRY) return;
  
  if (ENABLE_DEBUG) {
    Serial.print("[NATRouter] Packets forwarded: ");
    Serial.println(packets_forwarded);
    Serial.print("[NATRouter] Connected clients: ");
    Serial.println(wifi_manager->getConnectedClients());
  }
}

uint32_t NATRouter::getPacketsForwarded() const {
  return packets_forwarded;
}

void NATRouter::resetStats() {
  packets_forwarded = 0;
  last_packet_time = millis();
}
