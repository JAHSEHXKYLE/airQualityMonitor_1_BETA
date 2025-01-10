#include <WiFi.h>
#include <WebServer.h>
#include <ConfigHtml.h>

// Wi-Fi 配置
const char *ssid = "ESP32_AP";
const char *password = "12345678";

// 创建 Web 服务器
WebServer server(80);

uint16_t Tvoc_Data = 1111;
uint16_t Ch2o_Data = 2222;
uint16_t PM25_Data = 3333;
uint16_t CO2_Data = 4444;
float Temp_Data = 12.345;
float Hum_Data = 12.345;
uint16_t Press_Data = 1234.567;

// 返回 JSON 数据
void GetSensorData() {
  String json = "{";
  json += "\"tvoc\":" + String(Tvoc_Data) + ",";
  json += "\"ch2o\":" + String(Ch2o_Data) + ",";
  json += "\"pm25\":" + String(PM25_Data) + ",";
  json += "\"co2\":" + String(CO2_Data) + ",";
  json += "\"temperature\":" + String(Temp_Data, 1) + ",";
  json += "\"humidity\":" + String(Hum_Data, 1) + ",";
  json += "\"pressure\":" + String(Press_Data);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {

  // 启动串口调试
  Serial.begin(115200);

  // 启动 Wi-Fi AP 模式 
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // 配置 Web 服务器
  server.on("/", []() { server.send(200, "text/html", ConfigHtml::configHtml); });
  server.on("/GetSensorData", GetSensorData);
  server.begin();
  Serial.println("Web Server Started");
}

void loop() {
  server.handleClient();
}
