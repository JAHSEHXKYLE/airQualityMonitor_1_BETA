#include <WiFi.h>
#include <WebServer.h>

// 定义引脚
const int pin12 = 12;
const int pin13 = 13;
const int pin14 = 14;

// Wi-Fi 配置
const char *ssid = "ESP32_AP";
const char *password = "12345678";

// 创建 Web 服务器
WebServer server(80);

// 用于存储电压值的数组
float voltageArray[3] = {0, 0, 0};

// 读取引脚电压
void readVoltages() {
  voltageArray[0] = analogRead(pin12) * (3.3 / 4095.0); // 转换为电压
  voltageArray[1] = analogRead(pin13) * (3.3 / 4095.0);
  voltageArray[2] = analogRead(pin14) * (3.3 / 4095.0);
}

// 返回 JSON 数据
void handleGetVoltages() {
  readVoltages();
  String json = "{";
  json += "\"gpio12\":" + String(voltageArray[0], 2) + ",";
  json += "\"gpio13\":" + String(voltageArray[1], 2) + ",";
  json += "\"gpio14\":" + String(voltageArray[2], 2);
  json += "}";
  server.send(200, "application/json", json);
}

// HTML 页面
String generateHTML() {
  return R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 Voltage Monitor</title>
      <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
        h1 { color: #333; }
        .data { font-size: 1.5rem; color: #007BFF; }
      </style>
    </head>
    <body>
      <h1>ESP32 GPIO Voltage Monitor</h1>
      <p>GPIO 12 Voltage: <span class="data" id="gpio12">--</span> V</p>
      <p>GPIO 13 Voltage: <span class="data" id="gpio13">--</span> V</p>
      <p>GPIO 14 Voltage: <span class="data" id="gpio14">--</span> V</p>
      <script>
        async function updateVoltages() {
          try {
            const response = await fetch('/getVoltages');
            const data = await response.json();
            document.getElementById('gpio12').innerText = data.gpio12.toFixed(2);
            document.getElementById('gpio13').innerText = data.gpio13.toFixed(2);
            document.getElementById('gpio14').innerText = data.gpio14.toFixed(2);
          } catch (error) {
            console.error('Error fetching data:', error);
          }
        }
        setInterval(updateVoltages, 1000); // 每秒更新一次
        updateVoltages(); // 页面加载时立即更新
      </script>
    </body>
    </html>
  )rawliteral";
}

void setup() {
  // 初始化引脚
  pinMode(pin12, INPUT);
  pinMode(pin13, INPUT);
  pinMode(pin14, INPUT);

  // 启动串口调试
  Serial.begin(115200);

  // 启动 Wi-Fi AP 模式
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // 配置 Web 服务器
  server.on("/", []() { server.send(200, "text/html", generateHTML()); });
  server.on("/getVoltages", handleGetVoltages);
  server.begin();
  Serial.println("Web Server Started");
}

void loop() {
  // 处理客户端请求
  server.handleClient();
}
