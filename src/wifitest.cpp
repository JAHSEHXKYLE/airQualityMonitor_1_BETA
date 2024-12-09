#include <WiFi.h>
#include <WebServer.h>

// WiFi AP 模式的名称和密码
const char* ssid = "ESP32-C3-AP";
const char* password = "12345678";

// 创建 Web 服务器实例
WebServer server(80);

// ADC 引脚定义
const int analogPin = 12;  // GPIO 12

// HTML 页面
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Voltage Monitor</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    h1 { color: #333; }
    .voltage { font-size: 2em; color: #007BFF; margin-top: 20px; }
  </style>
  <script>
    function fetchVoltage() {
      fetch('/getVoltage')
        .then(response => response.text())
        .then(data => {
          document.getElementById('voltage').innerText = data + ' V';
        })
        .catch(error => console.error('Error:', error));
    }
    setInterval(fetchVoltage, 1000); // 每秒更新一次
  </script>
</head>
<body>
  <h1>Real-Time Voltage Monitor</h1>
  <div class="voltage" id="voltage">-- V</div>
</body>
</html>
)rawliteral";

// 处理网页请求
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 处理电压数据请求
void handleGetVoltage() {
  int adcValue = analogRead(analogPin);               // 读取 ADC 值
  float voltage = adcValue * (3.3 / 4095.0);          // 转换为电压 (假设 12 位 ADC 分辨率)
  String voltageStr = String(voltage, 2);             // 保留两位小数
  server.send(200, "text/plain", voltageStr);         // 返回电压值
}

void setup() {
  // 初始化串口
  Serial.begin(115200);

  // 设置 GPIO 为输入模式
  pinMode(analogPin, INPUT);

  // 初始化 WiFi AP 模式
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // 注册网页和数据请求的处理函数
  server.on("/", handleRoot);
  server.on("/getVoltage", handleGetVoltage);

  // 启动 Web 服务器
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // 处理客户端请求
  server.handleClient();
}
