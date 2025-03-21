#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

const char* AP_SSID = "ESP32_Config";
const char* AP_PASS = "12345678";

WebServer server(80);
Preferences preferences;
String currentSSID = "";
unsigned long lastReconnectAttempt = 0;

bool waitForConnection(unsigned int timeout);
void startAP();
void setupWebServer(); 
void handleRoot();
void handleConnect();
void handleStatus();

void setup() {
  Serial.begin(115200);
  preferences.begin("wifi-config", false);

  // 尝试自动连接已保存的WiFi
  String savedSSID = preferences.getString("ssid", "");
  String savedPass = preferences.getString("password", "");
  
  if (savedSSID != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    
    // 等待10秒连接
    if (waitForConnection(10000)) {
      Serial.println("Auto-connected to WiFi");
      Serial.println("ssid: " + savedSSID);
      Serial.println("IP: " + WiFi.localIP().toString());
      setupWebServer();
      return;
    }
  }
  
  // 连接失败时启动AP
  startAP();
}

void loop() {
  server.handleClient();
  
  // 每30秒检查一次连接状态
  if (millis() - lastReconnectAttempt > 30000) {
    if (WiFi.status() != WL_CONNECTED && currentSSID != "") {
      Serial.println("Reconnecting to WiFi...");
      WiFi.reconnect();
      lastReconnectAttempt = millis();
      
      if (!waitForConnection(15000)) {
        Serial.println("Reconnect failed, starting AP");
        startAP();
      }
    }
  }
}

bool waitForConnection(unsigned int timeout) {
  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (WiFi.status() == WL_CONNECTED) {
      currentSSID = WiFi.SSID();
      return true;
    }
    delay(500);
  }
  return false;
}

void startAP() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASS);
  setupWebServer();
  Serial.println("AP Mode IP: " + WiFi.softAPIP().toString());
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/connect", handleConnect);
  server.on("/status", handleStatus);
  server.begin();
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function updateStatus() {
      fetch('/status')
        .then(r => r.text())
        .then(t => {
          document.getElementById('status').innerHTML = t;
          setTimeout(updateStatus, 2000);
        });
    }
    
    function connectWiFi() {
      var ssid = document.getElementById('ssid').value;
      var pass = document.getElementById('password').value;
      
      fetch('/connect?ssid=' + encodeURIComponent(ssid) + 
            '&password=' + encodeURIComponent(pass))
        .then(r => r.text())
        .then(t => alert(t));
    }
  </script>
</head>
<body onload="updateStatus()">
  <h1>ESP32 WiFi配置</h1>
  <div>
    <input type="text" id="ssid" placeholder="WiFi名称">
    <input type="password" id="password" placeholder="密码">
    <button onclick="connectWiFi()">连接</button>
  </div>
  <h2>当前状态：</h2>
  <div id="status">加载中...</div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleConnect() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  
  if (ssid.isEmpty()) {
    server.send(400, "text/plain", "SSID required");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  
  if (waitForConnection(15000)) {
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    server.send(200, "text/plain", "Connected! IP: " + WiFi.localIP().toString());
    // 连接成功后关闭AP
    WiFi.mode(WIFI_STA);
    currentSSID = ssid;
  } else {
    server.send(500, "text/plain", "Connection failed");
    startAP();  // 失败后重新启用AP
  }
}

void handleStatus() {
  String status;
  if (WiFi.status() == WL_CONNECTED) {
    status = "Connected: " + currentSSID + "<br>IP: " + WiFi.localIP().toString();
  } else {
    status = "AP Mode IP: " + WiFi.softAPIP().toString();
  }
  server.send(200, "text/html", status);
}