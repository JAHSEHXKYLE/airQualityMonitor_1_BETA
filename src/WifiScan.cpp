#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>  //存储wifi信息
#include <SPIFFS.h>  //存储html文件

#define EEPROM_SIZE 500  // EEPROM 最大容量  每个数据由32字节的 ssid、63字节的密码、3个间隔字符、一个标记字符和一个结束字符组成 每个数据共占用100字节
                            // 所以 EEPROM 最大容量为 500/100 = 5 个数据

#define MAX_WIFI_COUNT 24 // web端显示的最大保存的 WiFi 数量 （最大不超过255个）

const char *AP_ssid = "AirMonitor_BETA";
const char *AP_password = "12345678";

struct WifiInfo {
    String ssid;
    uint8_t encType;
    int32_t rssi;
    uint8_t *bssid;
    int32_t channel;
    String password;
};
struct WifiList {
    uint8_t count;
    WifiInfo info[MAX_WIFI_COUNT];
};

WifiList wifiList;
WebServer server(80);

String GetWifiListjson();
void handleRoot();
void handleConnectWifi();

void saveWiFiData(String &data) {
    for (int i = 0; i < 5; i++){  // 遍历 5 个wifi信息块查找标记符位置
        char marker = EEPROM.read(i*100 + 99);
        if (marker == '1' && i != 4){  // 找到标记符位置
            EEPROM.write(i*100 + 99, '0');  // 标记符改为 0
            for (int j = 0; j < data.length(); j++) {  // 写入 data 到 EEPROM
                EEPROM.write((i+1)*100 + j, data[j]);
            }
            EEPROM.write((i+1)*100 + data.length(), '\0');  // 写入结束字符
            EEPROM.write((i+1)*100 + 99, '1');  // 更新标记符改为 1
            EEPROM.commit();  // 保存 EEPROM 数据
            return;
        } else if (marker == '1' && i == 4){ // 如果标记符在最后一个块中 则写入第一个块
            EEPROM.write(i*100 + 99, '0');
            for (int j = 0; j < data.length(); j++) {
                EEPROM.write(j, data[j]);
            }
            EEPROM.write(data.length(), '\0');
            EEPROM.write(99, '1');
            EEPROM.commit();
            return;
        }
    }
    // 如果没找到标记符位置 则写入第一个 块
    for (int j = 0; j < data.length(); j++) {
        EEPROM.write(j, data[j]);
    }
    EEPROM.write(data.length(), '\0');
    EEPROM.write(99, '1');
    EEPROM.commit();
    return;
}

void readEEPROMData() {
    char ch;
    for (int i = 0; i < 5; i++){  // 遍历 5 个wifi信息块
        String data = "";
        for (int j = 0; j < 100; j++) {  // 读取 100 字节数据
            ch = EEPROM.read(i*100 + j);
            data += ch;
        }
        Serial.print("Block " + String(i) + ": ");
        Serial.println(data);
    }
}

void handleRoot() {
    File htmlFile = SPIFFS.open("/index.html", "r");
    if (!htmlFile) {
        server.send(404, "text/plain", "File Not Found");
    }
    server.streamFile(htmlFile, "text/html");
    htmlFile.close();
}
void handleConnectWifi() {
    if (server.hasArg("plain")){
        String data = server.arg("plain");
        Serial.println(data);
        int separatorIndex = data.indexOf("霖");  // 无懈可击的分隔符
        if (separatorIndex != -1){
            String ssid = data.substring(0, separatorIndex);
            String password = data.substring(separatorIndex + 3);
            Serial.print("SSID:");
            Serial.println(ssid);
            Serial.print("Password:");
            Serial.println(password);
            for (int i = 0; wifiList.count > i; i++) {
                if (wifiList.info[i].ssid == ssid) {
                    wifiList.info[i].password = password;
                    saveWiFiData(data);
                    readEEPROMData();
                    break;
                }
            }

            server.send(200, "text/plain", "获取wifi信息成功");
        } else{
            Serial.println("Invalid data");
            server.send(400, "text/plain", "Invalid data");
        }
    }else{
        server.send(400, "text/plain", "Invalid request");
    }
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
    wifiList.count = count;
    if (count == 0) {
        json += "}"; // 没有找到 WiFi 则返回空列表
        return json;
    } else{
        for (uint8_t i = 0; i < count; i++) {
            if (i >= MAX_WIFI_COUNT) {break;}  // 限制网页内显示的 WiFi 数量
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
    if (!EEPROM.begin(EEPROM_SIZE)){
        Serial.println("EEPROM Begin Failed");
        return;
    }
    
    WiFi.softAP(AP_ssid, AP_password);
    // WiFi.begin(ssid, password);
    // while (WiFi.status() != WL_CONNECTED) { // 等待连接WIFI 直到连接成功 退出循环
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println(WiFi.localIP());
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i , '-');
    }
    EEPROM.commit();

    server.on("/", handleRoot); 
    server.on("/connect_wifi", handleConnectWifi);
    server.on("/get_wifi_data", []() {server.send(200, "application/json", GetWifiListjson());});
    server.begin();
}

void loop() {
    server.handleClient();
}
