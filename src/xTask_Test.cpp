#include <Arduino.h>
#include <WiFi.h>

QueueHandle_t sensorQueue;
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Core 0：传感器任务
void sensorTask(void *pvParams) {
  while(1) {
    float temp = analogRead(4) * 3.3 / 4095.0 * 100; // 模拟温度值
    xQueueSend(sensorQueue, &temp, 0);
    delay(100);
  }
}

// Core 1：网络任务
void networkTask(void *pvParams) {
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  while(1) {
    float temp;
    if(xQueueReceive(sensorQueue, &temp, portMAX_DELAY)) {
      Serial.print("上传温度: ");
      Serial.println(temp);
      // 实际可添加HTTP客户端代码
    }
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  sensorQueue = xQueueCreate(10, sizeof(float));

  xTaskCreatePinnedToCore(
    sensorTask,
    "Sensor",
    4096,
    NULL,
    2,  // 高优先级
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    networkTask,
    "Network",
    8192,  // 需要更大堆栈
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {}