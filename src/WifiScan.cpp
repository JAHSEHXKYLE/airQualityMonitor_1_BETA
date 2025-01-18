#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <SPIFFS.h>  //存储html文件
#include "ConfigHtml.h"

#define SSID_SIZE 32
#define PASSWORD_SIZE 64
#define MAX_WIFI_COUNT 12 // 最大保存的 WiFi 数量 最大不超过255个

const char *ssid = "YANG_PC";
const char *password = "K2003@0315#";

struct WifiInfo {
    String ssid;
    uint8_t encType;
    int32_t rssi;
    uint8_t *bssid;
    int32_t channel;
};

struct WifiList {
    WifiInfo info[MAX_WIFI_COUNT];
};

WifiList wifiList;
WebServer server(80);

String GetWifiListjson();
void handleRoot();

void handleRoot() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File Not Found");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

String GetWifiListjson() {
    String json = "{";
    String wifi_names = "\"wifi_names\":[" , wifi_encryptedTYPE = "\"wifi_encryptedTYPE\":[" , wifi_channel = "\"wifi_channel\":[" , wifi_rssi = "\"wifi_rssi\":[" , wifi_MAC  = "\"wifi_MAC\":[" ;
    String envTYPE[] = {
        "OPEN" , 
        "WEP" , 
        "WPA_PSK" ,
        "WPA2_PSK" ,
        "WPA_WPA2_PSK" ,
        "ENTERPRISE" ,
        "WPA3_PSK" ,
        "WPA2_WPA3_PSK" ,
        "WAPI_PSK" ,
        "WPA3_ENT_192" ,
        "MAX"
    };
    uint8_t count = WiFi.scanNetworks();
    if (count == 0) {
        json += "}"; // 没有找到 WiFi 则返回空列表
        return json;
    } else{
        for (uint8_t i = 0; i < count; i++) {
            if (WiFi.getNetworkInfo(i, wifiList.info[i].ssid, wifiList.info[i].encType, wifiList.info[i].rssi, wifiList.info[i].bssid, wifiList.info[i].channel)) {
                wifi_names += "\"" + wifiList.info[i].ssid + "\",";
                wifi_encryptedTYPE += "\"" + envTYPE[wifiList.info[i].encType] + "\",";
                wifi_channel += "\"" + String(wifiList.info[i].channel) + "\",";
                wifi_rssi += "\"" + String(wifiList.info[i].rssi) + "\",";
                char bssidStr[18]; // 18 字符用于存储 BSSID 的字符串表示，包括冒号和终止符
                sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
                        wifiList.info[i].bssid[0], wifiList.info[i].bssid[1], wifiList.info[i].bssid[2], 
                        wifiList.info[i].bssid[3], wifiList.info[i].bssid[4], wifiList.info[i].bssid[5]);
                wifi_MAC += "\"" + String(bssidStr) + "\",";
            }
        }
        json += wifi_names.substring(0, wifi_names.length() - 1) + "],";
        json += wifi_encryptedTYPE.substring(0, wifi_encryptedTYPE.length() - 1) + "],";
        json += wifi_channel.substring(0, wifi_channel.length() - 1) + "],";
        json += wifi_rssi.substring(0, wifi_rssi.length() - 1) + "],";
        json += wifi_MAC.substring(0, wifi_MAC.length() - 1) + "]";
        json += "}";
        return json;
    }
}

void setup() {
    Serial.begin(115200);
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { // 等待连接WIFI 直到连接成功 退出循环
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
    server.on("/", []() {server.send(200, "text/html", ConfigHtml::initHtml);}); 
    server.on("/get_wifi_data", []() {server.send(200, "application/json", GetWifiListjson());});
    server.begin();
}
    

void loop() {
    server.handleClient();
}
