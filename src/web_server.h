#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <String.h>

class WiFiManager;

class WebServer {
public:
  explicit WebServer(WiFiManager* wm);
  ~WebServer();
  
  void init();
  void update();
  void stop();
  
  bool isSlowModeEnabled() const;
  void setSlowMode(bool enabled);
  
private:
  WiFiManager* wifi_manager;
  bool slow_mode;
  bool server_running;
  
  // HTTP request handlers
  void handleRoot();
  void handleConfigPage();
  void handleCaptive();
  void handleApiStatus();
  void handleApiConfigSet();
  void handleApiToggleSlow();
  void handleAdmin();
  void handleNotFound();
  
  // HTML generation
  String getIndexHTML();
  String getConfigPageHTML();
};

#endif // WEB_SERVER_H
