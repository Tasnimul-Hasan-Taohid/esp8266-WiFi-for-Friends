#ifndef NAT_ROUTER_H
#define NAT_ROUTER_H

#include <stdint.h>

class WiFiManager;
class WebServer;

class NATRouter {
public:
  NATRouter(WiFiManager* wm, WebServer* ws);
  
  void init();
  void update();
  
  uint32_t getPacketsForwarded() const;
  void resetStats();
  
private:
  WiFiManager* wifi_manager;
  WebServer* web_server;
  
  uint32_t packets_forwarded;
  uint32_t last_packet_time;
  uint32_t current_delay_ms;
  
  void logStats();
};

#endif // NAT_ROUTER_H
