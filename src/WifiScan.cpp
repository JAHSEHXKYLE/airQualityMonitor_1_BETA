#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  
  // 初始化WiFi为Station模式
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("正在扫描网络...");
  int networkCount = WiFi.scanNetworks();

  if (networkCount == 0) {
    Serial.println("未找到任何网络");
  } else {
    Serial.print(networkCount);
    Serial.println(" 个网络已找到：");
    for (int i = 0; i < networkCount; ++i) {
      Serial.print("网络名称：");
      Serial.println(WiFi.SSID(i));
      Serial.print("信号强度：");
      Serial.println(WiFi.RSSI(i));
      Serial.println("-----------------------");
    }
  }
}

void loop() {
  // 暂时没有内容
}
