#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YANG_PC";       // 修改为你的 WiFi SSID
const char* password = "K2003@0315#"; // 修改为你的 WiFi 密码

WebServer server(80);

// 提供 HTML 页面
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 WiFi Config</title>
      <script>
        async function sendWiFiConfig() {
          const wifiName = document.getElementById("wifiName").value;
          const wifiPassword = document.getElementById("wifiPassword").value;
          const data = wifiName + "/!@#" + wifiPassword;

          try {
            const response = await fetch("/submit", {
              method: "POST",
              headers: { "Content-Type": "text/plain" },
              body: data
            });

            if (response.ok) {
              const result = await response.text();
              alert("ESP32 Response: " + result);
            } else {
              alert("Failed to send data. Status: " + response.status);
            }
          } catch (error) {
            console.error("Error:", error);
          }
        }
      </script>
    </head>
    <body>
      <h1>Configure WiFi</h1>
      <label for="wifiName">WiFi Name:</label>
      <input type="text" id="wifiName" placeholder="Enter WiFi Name"><br><br>
      <label for="wifiPassword">WiFi Password:</label>
      <input type="password" id="wifiPassword" placeholder="Enter WiFi Password"><br><br>
      <button onclick="sendWiFiConfig()">Submit</button>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// 处理 WiFi 配置信息的 POST 请求
void handleSubmit() {
  if (server.hasArg("plain")) {
    String data = server.arg("plain");
    Serial.println("Received Data: " + data);

    // 解析数据
    int separatorIndex = data.indexOf("/!@#");
    if (separatorIndex != -1) {
      String wifiName = data.substring(0, separatorIndex);
      String wifiPassword = data.substring(separatorIndex + 4);

      Serial.println("WiFi Name: " + wifiName);
      Serial.println("WiFi Password: " + wifiPassword);

      // 返回成功响应
      server.send(200, "text/plain", "Data received successfully!");
    } else {
      server.send(400, "text/plain", "Invalid format");
    }
  } else {
    server.send(400, "text/plain", "No data received");
  }
}

void setup() {
  Serial.begin(115200);

  // 连接到 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected! IP address: " + WiFi.localIP().toString());

  // 设置路由
  server.on("/", handleRoot);         // 提供 HTML 页面
  server.on("/submit", HTTP_POST, handleSubmit); // 处理 WiFi 配置

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
