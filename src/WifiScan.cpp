#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

#define SSID_SIZE 32
#define PASSWORD_SIZE 64
#define MAX_WIFI_COUNT 12 // 最大保存的 WiFi 数量 最大不超过255个

const char *ssid = "ESP32_test";
const char *password = "12345678";

struct WifiInfo {
    String ssid;
    uint8_t encType;
    int32_t rssi;
    uint8_t *bssid;
    int32_t channel;
};

struct WifiList {
    uint8_t count;
    WifiInfo info[MAX_WIFI_COUNT];
};

WifiList wifiList;
WebServer server(80);

void GetWifiList();

void GetWifiList() {
    uint8_t count = WiFi.scanNetworks();
    wifiList.count = count;
    for (uint8_t i = 0; i < count; i++) {
        if (WiFi.getNetworkInfo(i, wifiList.info[i].ssid, wifiList.info[i].encType, wifiList.info[i].rssi, wifiList.info[i].bssid, wifiList.info[i].channel)) {
            Serial.println("ssid: " + wifiList.info[i].ssid);
            Serial.println("rssi: " + String(wifiList.info[i].rssi));
            Serial.println("channel: " + String(wifiList.info[i].channel));
            Serial.println("encType: " + String(wifiList.info[i].encType));
            
            // 将 bssid 数组转换为字符串格式
            char bssidStr[18]; // 18 字符用于存储 BSSID 的字符串表示，包括冒号和终止符
            sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
                    wifiList.info[i].bssid[0], wifiList.info[i].bssid[1], wifiList.info[i].bssid[2], 
                    wifiList.info[i].bssid[3], wifiList.info[i].bssid[4], wifiList.info[i].bssid[5]);
            Serial.println("bssid: " + String(bssidStr) + "\n\n\n");
        }
    }
}
void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("WiFi Scan...\n");
    GetWifiList();
    Serial.println("WiFi Scan Done.");
}
    

void loop() {
  // 暂时没有内容
}
