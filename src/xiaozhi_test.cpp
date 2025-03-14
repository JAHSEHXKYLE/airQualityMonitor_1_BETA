#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 配置项 --------------------------------------------------------
const char* SSID = "YANG_PC";
const char* PASSWORD = "K2003@0315#";
const char* OTA_VERSION_URL = "https://api.tenclass.net/xiaozhi/ota/";

// 根证书 (DigiCert Global Root CA)
const char* ROOT_CA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----";

// 全局变量 ------------------------------------------------------
String mqtt_info;
String MAC_ADDR;

// 辅助函数：添加分区信息 -----------------------------------------
void addPartition(JsonArray& array, const char* label, int type, int subtype, long address, long size) {
  JsonObject obj = array.createNestedObject();
  obj["label"] = label;
  obj["type"] = type;
  obj["subtype"] = subtype;
  obj["address"] = address;
  obj["size"] = size;
}

// 内存监控 ------------------------------------------------------
void check_heap(const char* tag) {
  Serial.printf("[%s] Free Heap: %d, Min: %d\n", tag, ESP.getFreeHeap(), ESP.getMinFreeHeap());
}

// 核心函数：获取OTA版本信息 ------------------------------------
void get_ota_version() {
  check_heap("Start");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi未连接");
    return;
  }

  WiFiClientSecure client;
  HTTPClient http;
  
  // 配置HTTPS
  client.setCACert(ROOT_CA);
  http.begin(client, OTA_VERSION_URL);
  
  // 设置Headers
  http.addHeader("Device-Id", MAC_ADDR);
  http.addHeader("Content-Type", "application/json");

  // 构建JSON数据 -------------------------------------------------
  DynamicJsonDocument doc(4096);

  // 基础信息
  doc["flash_size"] = ESP.getFlashChipSize();
  doc["minimum_free_heap_size"] = ESP.getFreeHeap();
  doc["mac_address"] = MAC_ADDR;
  doc["chip_model_name"] = "esp32s3";

  // 芯片信息
  JsonObject chip_info = doc.createNestedObject("chip_info");
  chip_info["model"] = 9;
  chip_info["cores"] = 2;
  chip_info["revision"] = 2;
  chip_info["features"] = 18;

  // 应用信息
  JsonObject application = doc.createNestedObject("application");
  application["name"] = "xiaozhi";
  application["version"] = "0.9.9";
  application["compile_time"] = "Jan 22 2025T20:40:23Z";
  application["idf_version"] = "v5.3.2-dirty";
  application["elf_sha256"] = "22986216df095587c42f8aeb06b239781c68ad8df80321e260556da7fcf5f522";

  // 分区表信息
  JsonArray partition_table = doc.createNestedArray("partition_table");
  addPartition(partition_table, "nvs", 1, 2, 36864, 16384);
  addPartition(partition_table, "otadata", 1, 0, 53248, 8192);
  addPartition(partition_table, "phy_init", 1, 1, 61440, 4096);
  addPartition(partition_table, "model", 1, 130, 65536, 983040);
  addPartition(partition_table, "storage", 1, 130, 1048576, 1048576);
  addPartition(partition_table, "factory", 0, 0, 2097152, 4194304);
  addPartition(partition_table, "ota_0", 0, 16, 6291456, 4194304);
  addPartition(partition_table, "ota_1", 0, 17, 10485760, 4194304);

  // OTA信息
  doc["ota"]["label"] = "factory";

  // 主板信息
  JsonObject board = doc.createNestedObject("board");
  board["type"] = "bread-compact-wifi";
  board["ssid"] = WiFi.SSID();
  board["rssi"] = WiFi.RSSI();
  board["channel"] = WiFi.channel();
  board["ip"] = WiFi.localIP().toString();
  board["mac"] = MAC_ADDR;

  // 序列化并发送请求 ---------------------------------------------
  String payload;
  serializeJson(doc, payload);
  check_heap("After JSON");

  Serial.println("发送请求:");
  serializeJsonPretty(doc, Serial);
  Serial.println("\n----------------------------------");

  int httpCode = http.POST(payload);
  
  // 处理响应 -----------------------------------------------------
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println("收到响应:");
      Serial.println(response);

      DynamicJsonDocument resDoc(1024);
      DeserializationError error = deserializeJson(resDoc, response);
      
      if (!error) {
        mqtt_info = resDoc["mqtt"].as<String>();
        Serial.print("MQTT信息: ");
        Serial.println(mqtt_info);
      } else {
        Serial.print("JSON解析失败: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.printf("HTTP错误码: %d\n", httpCode);
      Serial.print("响应内容: ");
      Serial.println(http.getString());
    }
  } else {
    Serial.printf("HTTP请求失败: %s\n", http.errorToString(httpCode).c_str());
  }

  // 清理资源 -----------------------------------------------------
  http.end();
  client.stop();
  check_heap("End");
}

void setup() {
  Serial.begin(115200);
  
  // 连接WiFi
  Serial.print("连接WiFi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi已连接!");
  
  // 获取MAC地址
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  MAC_ADDR = String(macStr);
  Serial.print("MAC地址: ");
  Serial.println(MAC_ADDR);

  // 获取OTA信息
  get_ota_version();
}

void loop() {
  // 主循环保持空
}