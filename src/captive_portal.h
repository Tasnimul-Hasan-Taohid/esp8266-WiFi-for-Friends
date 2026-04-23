#ifndef CAPTIVE_PORTAL_H
#define CAPTIVE_PORTAL_H

class WiFiManager;
class DNSServer;

class CaptivePortal {
public:
  explicit CaptivePortal(WiFiManager* wm);
  ~CaptivePortal();
  
  void init();
  void update();
  void stop();
  
private:
  WiFiManager* wifi_manager;
  bool dns_initialized;
};

#endif // CAPTIVE_PORTAL_H
