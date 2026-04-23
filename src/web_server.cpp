#include "web_server.h"
#include "config.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer* server = nullptr;
WebServer* ws = nullptr;

WebServer::WebServer(WiFiManager* wm)
    : wifi_manager(wm), 
      slow_mode(DEFAULT_SLOW_MODE),
      server_running(false) {
}

void WebServer::init() {
  if (server_running) return;
  
  server = new ESP8266WebServer(WEB_SERVER_PORT);
  
  // Setup routes
  server->on("/", HTTP_GET, [this]() { handleRoot(); });
  server->on("/config", HTTP_GET, [this]() { handleConfigPage(); });
  server->on("/api/status", HTTP_GET, [this]() { handleApiStatus(); });
  server->on("/api/config", HTTP_POST, [this]() { handleApiConfigSet(); });
  server->on("/api/toggle-slow", HTTP_POST, [this]() { handleApiToggleSlow(); });
  server->on("/captive", HTTP_GET, [this]() { handleCaptive(); });
  server->on("/admin", HTTP_GET, [this]() { handleAdmin(); });
  server->onNotFound([this]() { handleNotFound(); });
  
  server->begin();
  server_running = true;
  
  if (ENABLE_DEBUG) {
    Serial.println("[WebServer] HTTP server started on port 80");
  }
}

void WebServer::update() {
  if (server_running && server) {
    server->handleClient();
  }
}

void WebServer::stop() {
  if (server) {
    server->stop();
    delete server;
    server = nullptr;
  }
  server_running = false;
  
  if (ENABLE_DEBUG) {
    Serial.println("[WebServer] HTTP server stopped");
  }
}

void WebServer::handleRoot() {
  String html = getIndexHTML();
  server->send(200, "text/html; charset=utf-8", html);
}

void WebServer::handleConfigPage() {
  String html = getConfigPageHTML();
  server->send(200, "text/html; charset=utf-8", html);
}

void WebServer::handleCaptive() {
  String response = "<html><head>";
  response += "<meta http-equiv=\"refresh\" content=\"0;url=http://192.168.4.1/\" />";
  response += "</head><body></body></html>";
  server->send(200, "text/html", response);
}

void WebServer::handleApiStatus() {
  String json = "{";
  json += "\"sta_connected\":" + String(wifi_manager->isSTAConnected() ? "true" : "false") + ",";
  json += "\"sta_ssid\":\"" + String(wifi_manager->getSTASSID()) + "\",";
  json += "\"sta_ip\":\"" + wifi_manager->getSTAIP().toString() + "\",";
  json += "\"sta_signal\":" + String(wifi_manager->getSTASignal()) + ",";
  json += "\"ap_running\":" + String(wifi_manager->isAPRunning() ? "true" : "false") + ",";
  json += "\"ap_ip\":\"" + wifi_manager->getAPIP().toString() + "\",";
  json += "\"ap_clients\":" + String(wifi_manager->getConnectedClients()) + ",";
  json += "\"slow_mode\":" + String(slow_mode ? "true" : "false") + ",";
  json += "\"config_mode\":" + String(wifi_manager->isConfigMode() ? "true" : "false");
  json += "}";
  
  server->send(200, "application/json", json);
}

void WebServer::handleApiConfigSet() {
  if (server->hasArg("plain")) {
    String body = server->arg("plain");
    
    // Simple JSON parsing for ssid, password, ap_ssid
    int ssid_start = body.indexOf("\"ssid\":\"") + 8;
    int ssid_end = body.indexOf("\"", ssid_start);
    String ssid = body.substring(ssid_start, ssid_end);
    
    int pwd_start = body.indexOf("\"password\":\"") + 12;
    int pwd_end = body.indexOf("\"", pwd_start);
    String password = body.substring(pwd_start, pwd_end);
    
    int ap_start = body.indexOf("\"ap_ssid\":\"") + 11;
    int ap_end = body.indexOf("\"", ap_start);
    String ap_ssid = body.substring(ap_start, ap_end);
    
    if (ssid.length() > 0 && password.length() > 0 && ap_ssid.length() > 0) {
      wifi_manager->setWiFiConfig(ssid.c_str(), password.c_str(), ap_ssid.c_str());
      server->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"WiFi configuration updated\"}");
      return;
    }
  }
  
  server->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid request\"}");
}

void WebServer::handleApiToggleSlow() {
  slow_mode = !slow_mode;
  String json = "{\"status\":\"ok\",\"slow_mode\":" + String(slow_mode ? "true" : "false") + "}";
  server->send(200, "application/json", json);
}

void WebServer::handleAdmin() {
  String html = "<html><head><title>Admin Panel</title></head><body>";
  html += "<h1>Admin Panel</h1>";
  html += "<p>STA IP: " + wifi_manager->getSTAIP().toString() + "</p>";
  html += "<p>AP IP: " + wifi_manager->getAPIP().toString() + "</p>";
  html += "<p>Connected Clients: " + String(wifi_manager->getConnectedClients()) + "</p>";
  html += "<p>Signal Strength: " + String(wifi_manager->getSTASignal()) + " dBm</p>";
  html += "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
  html += "<p>Slow Mode: " + String(slow_mode ? "ON" : "OFF") + "</p>";
  html += "</body></html>";
  
  server->send(200, "text/html", html);
}

void WebServer::handleNotFound() {
  server->sendHeader("Location", "http://192.168.4.1/");
  server->send(302, "text/plain", "");
}

bool WebServer::isSlowModeEnabled() const {
  return slow_mode;
}

void WebServer::setSlowMode(bool enabled) {
  slow_mode = enabled;
}

String WebServer::getIndexHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WiFi Repeater Dashboard</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .container {
      background: white;
      border-radius: 12px;
      box-shadow: 0 10px 40px rgba(0,0,0,0.2);
      max-width: 500px;
      width: 100%;
      padding: 30px;
    }
    h1 {
      color: #333;
      margin-bottom: 10px;
      font-size: 24px;
    }
    .subtitle {
      color: #666;
      font-size: 14px;
      margin-bottom: 30px;
    }
    .status-section {
      margin-bottom: 30px;
      padding: 20px;
      background: #f8f9fa;
      border-radius: 8px;
      border-left: 4px solid #667eea;
    }
    .status-item {
      margin: 12px 0;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .status-label {
      color: #666;
      font-weight: 500;
    }
    .status-value {
      color: #333;
      font-weight: 600;
      word-break: break-all;
    }
    .indicator {
      width: 12px;
      height: 12px;
      border-radius: 50%;
      display: inline-block;
    }
    .indicator.connected {
      background: #10b981;
    }
    .indicator.disconnected {
      background: #ef4444;
    }
    .button-group {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-top: 20px;
    }
    button {
      padding: 12px 20px;
      border: none;
      border-radius: 6px;
      font-size: 14px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s ease;
    }
    .btn-primary {
      background: #667eea;
      color: white;
    }
    .btn-primary:hover {
      background: #5568d3;
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
    }
    .btn-secondary {
      background: #f3f4f6;
      color: #333;
      border: 2px solid #e5e7eb;
    }
    .btn-secondary:hover {
      background: #e5e7eb;
    }
    .slow-mode-toggle {
      background: white;
      border: 2px solid #fbbf24;
      color: #f59e0b;
      margin-top: 10px;
    }
    .slow-mode-toggle.active {
      background: #fbbf24;
      color: white;
    }
    .modal {
      display: none;
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0,0,0,0.5);
      z-index: 1000;
      align-items: center;
      justify-content: center;
    }
    .modal.active {
      display: flex;
    }
    .modal-content {
      background: white;
      padding: 30px;
      border-radius: 12px;
      max-width: 400px;
      width: 90%;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
    }
    .modal-content h2 {
      margin-bottom: 20px;
      color: #333;
    }
    .form-group {
      margin-bottom: 15px;
    }
    label {
      display: block;
      margin-bottom: 5px;
      color: #666;
      font-weight: 500;
      font-size: 14px;
    }
    input {
      width: 100%;
      padding: 10px 12px;
      border: 2px solid #e5e7eb;
      border-radius: 6px;
      font-size: 14px;
      transition: border-color 0.3s ease;
    }
    input:focus {
      outline: none;
      border-color: #667eea;
    }
    .modal-buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-top: 20px;
    }
    .close-btn {
      background: #ef4444;
      color: white;
    }
    .close-btn:hover {
      background: #dc2626;
    }
    .alert {
      padding: 12px 16px;
      border-radius: 6px;
      margin-bottom: 15px;
      font-size: 14px;
    }
    .alert.success {
      background: #d1fae5;
      color: #065f46;
      border-left: 4px solid #10b981;
    }
    .alert.error {
      background: #fee2e2;
      color: #7f1d1d;
      border-left: 4px solid #ef4444;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🌐 WiFi Repeater</h1>
    <p class="subtitle">Free Guest Network Manager</p>
    
    <div id="alert-container"></div>
    
    <div class="status-section">
      <h3 style="margin-bottom: 15px; color: #333;">Primary Network</h3>
      
      <div class="status-item">
        <span class="status-label">Status</span>
        <span>
          <span class="indicator disconnected" id="sta-indicator"></span>
          <span id="sta-status">Disconnected</span>
        </span>
      </div>
      
      <div class="status-item">
        <span class="status-label">Network</span>
        <span class="status-value" id="sta-ssid">—</span>
      </div>
      
      <div class="status-item">
        <span class="status-label">IP Address</span>
        <span class="status-value" id="sta-ip">—</span>
      </div>
      
      <div class="status-item">
        <span class="status-label">Signal</span>
        <span class="status-value" id="sta-signal">—</span>
      </div>
    </div>
    
    <div class="status-section">
      <h3 style="margin-bottom: 15px; color: #333;">Guest Network</h3>
      
      <div class="status-item">
        <span class="status-label">Status</span>
        <span>
          <span class="indicator connected" id="ap-indicator"></span>
          <span id="ap-status">Active</span>
        </span>
      </div>
      
      <div class="status-item">
        <span class="status-label">SSID</span>
        <span class="status-value" id="ap-ssid">—</span>
      </div>
      
      <div class="status-item">
        <span class="status-label">IP Address</span>
        <span class="status-value" id="ap-ip">—</span>
      </div>
      
      <div class="status-item">
        <span class="status-label">Connected Clients</span>
        <span class="status-value" id="ap-clients">0</span>
      </div>
    </div>
    
    <div class="button-group">
      <button class="btn-primary" onclick="openConfigModal()">⚙️ Configure</button>
      <button class="btn-secondary slow-mode-toggle" id="slow-mode-btn" onclick="toggleSlowMode()">🐢 Slow Mode OFF</button>
    </div>
  </div>
  
  <div id="configModal" class="modal">
    <div class="modal-content">
      <h2>WiFi Configuration</h2>
      
      <div class="form-group">
        <label for="ssid">Primary Network SSID</label>
        <input type="text" id="ssid" placeholder="Enter WiFi name" required>
      </div>
      
      <div class="form-group">
        <label for="password">Password</label>
        <input type="password" id="password" placeholder="Enter WiFi password" required>
      </div>
      
      <div class="form-group">
        <label for="ap-ssid-input">Guest Network Name</label>
        <input type="text" id="ap-ssid-input" placeholder="e.g., Free_Guest_WiFi" required>
      </div>
      
      <div class="modal-buttons">
        <button class="btn-primary" onclick="saveConfig()">Save</button>
        <button class="close-btn" onclick="closeConfigModal()">Cancel</button>
      </div>
    </div>
  </div>
  
  <script>
    const updateInterval = 3000;
    let statusData = {};
    
    async function updateStatus() {
      try {
        const response = await fetch('/api/status');
        const data = await response.json();
        statusData = data;
        
        const staConnected = data.sta_connected;
        document.getElementById('sta-indicator').className = 
          'indicator ' + (staConnected ? 'connected' : 'disconnected');
        document.getElementById('sta-status').textContent = 
          staConnected ? 'Connected' : 'Disconnected';
        document.getElementById('sta-ssid').textContent = 
          staConnected ? data.sta_ssid : '—';
        document.getElementById('sta-ip').textContent = 
          staConnected ? data.sta_ip : '—';
        document.getElementById('sta-signal').textContent = 
          staConnected ? (data.sta_signal + ' dBm') : '—';
        
        const apRunning = data.ap_running;
        document.getElementById('ap-indicator').className = 
          'indicator ' + (apRunning ? 'connected' : 'disconnected');
        document.getElementById('ap-status').textContent = 
          apRunning ? 'Active' : 'Inactive';
        document.getElementById('ap-ssid').textContent = 
          apRunning ? data.ap_ssid : '—';
        document.getElementById('ap-ip').textContent = 
          apRunning ? data.ap_ip : '—';
        document.getElementById('ap-clients').textContent = data.ap_clients;
        
        const slowModeBtn = document.getElementById('slow-mode-btn');
        if (data.slow_mode) {
          slowModeBtn.textContent = '🐢 Slow Mode ON';
          slowModeBtn.classList.add('active');
        } else {
          slowModeBtn.textContent = '🐢 Slow Mode OFF';
          slowModeBtn.classList.remove('active');
        }
      } catch (error) {
        console.error('Status update error:', error);
      }
    }
    
    function openConfigModal() {
      document.getElementById('configModal').classList.add('active');
      if (statusData.sta_ssid) {
        document.getElementById('ssid').value = statusData.sta_ssid;
      }
      document.getElementById('ap-ssid-input').value = statusData.ap_ssid || 'Free_Guest_WiFi';
    }
    
    function closeConfigModal() {
      document.getElementById('configModal').classList.remove('active');
    }
    
    async function saveConfig() {
      const ssid = document.getElementById('ssid').value;
      const password = document.getElementById('password').value;
      const apSsid = document.getElementById('ap-ssid-input').value;
      
      if (!ssid || !password || !apSsid) {
        showAlert('Please fill in all fields', 'error');
        return;
      }
      
      try {
        const response = await fetch('/api/config', {
          method: 'POST',
          body: JSON.stringify({ ssid, password, ap_ssid: apSsid })
        });
        
        const data = await response.json();
        if (response.ok) {
          showAlert('Configuration saved! Connecting...', 'success');
          closeConfigModal();
          setTimeout(updateStatus, 2000);
        } else {
          showAlert('Error: ' + (data.message || 'Unknown error'), 'error');
        }
      } catch (error) {
        showAlert('Network error: ' + error.message, 'error');
      }
    }
    
    async function toggleSlowMode() {
      try {
        const response = await fetch('/api/toggle-slow', { method: 'POST' });
        const data = await response.json();
        updateStatus();
      } catch (error) {
        console.error('Toggle slow mode error:', error);
      }
    }
    
    function showAlert(message, type) {
      const container = document.getElementById('alert-container');
      const alert = document.createElement('div');
      alert.className = 'alert ' + type;
      alert.textContent = message;
      container.appendChild(alert);
      
      setTimeout(() => alert.remove(), 4000);
    }
    
    updateStatus();
    setInterval(updateStatus, updateInterval);
    
    window.onclick = (event) => {
      const modal = document.getElementById('configModal');
      if (event.target === modal) {
        closeConfigModal();
      }
    };
  </script>
</body>
</html>
)rawliteral";
}

String WebServer::getConfigPageHTML() {
  return getIndexHTML();
}

WebServer::~WebServer() {
  stop();
}
