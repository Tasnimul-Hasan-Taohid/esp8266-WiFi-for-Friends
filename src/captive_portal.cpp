#include "captive_portal.h"
#include "config.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h>

DNSServer* dns_server = nullptr;

CaptivePortal::CaptivePortal(WiFiManager* wm)
    : wifi_manager(wm), dns_initialized(false) {
}

void CaptivePortal::init() {
  if (dns_initialized) return;
  
  // Create DNS server
  dns_server = new DNSServer();
  
  // Start DNS server on port 53
  // Redirect all DNS requests to AP IP
  dns_server->setErrorReplyCode(DNSReplyCode::NoError);
  dns_server->start(53, "*", WiFi.softAPIP());
  
  dns_initialized = true;
  
  if (ENABLE_DEBUG) {
    Serial.println("[CaptivePortal] DNS server started");
  }
}

void CaptivePortal::update() {
  if (dns_initialized && dns_server) {
    dns_server->processNextRequest();
  }
}

void CaptivePortal::stop() {
  if (dns_server) {
    dns_server->stop();
    delete dns_server;
    dns_server = nullptr;
  }
  dns_initialized = false;
  
  if (ENABLE_DEBUG) {
    Serial.println("[CaptivePortal] DNS server stopped");
  }
}

CaptivePortal::~CaptivePortal() {
  stop();
}
