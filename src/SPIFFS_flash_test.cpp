#include <SPIFFS.h>

void listFiles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  Serial.println("SPIFFS 中的文件列表：");
  while (file) {
    Serial.print("文件名: ");
    Serial.println(file.name());
    Serial.print("文件大小: ");
    Serial.println(file.size());
    file = root.openNextFile();
  }
}

void setup() {
  Serial.begin(115200);

  // 初始化 SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS 初始化失败");
    return;
  }
  Serial.println("SPIFFS 初始化成功");

  listFiles();
  Serial.println("扫描完成！");
}

void loop() {
  // 空
}
