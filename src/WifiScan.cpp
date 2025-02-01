#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>  //存储wifi信息
#include <SPIFFS.h>  //存储html文件

#define EEPROM_SIZE 500  // EEPROM 最大容量  每个数据由32字节的 ssid、63字节的密码、3个间隔字符、一个标记字符和一个结束字符组成 每个数据共占用100字节
                            // 所以 EEPROM 最大容量为 500/100 = 5 个数据

#define MAX_WIFI_COUNT 24 // web端显示的最大保存的 WiFi 数量 （最大不超过255个）

const char *AP_ssid = "AirMonitor_BETA";
const char *AP_password = "12345678";
String ssidsFromEEPROM[5];   // 存放从 EEPROM 读取的 ssid 信息
String passwordsFromEEPROM[5];

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

String GetWifiListjson();
void handleRoot();
void handleConnectWifi();

void saveWiFiData(String &ssid, String &password) {
    String data = ssid + "霖" + password;
    char ch;
    for (int i = 0; i < 5; i++){
        String EEPROM_Data = "";
        for (int j = 0; j < 100; j++){
            ch = EEPROM.read(i*100 + j);
            EEPROM_Data += ch;
        }
        if (EEPROM_Data.indexOf(ssid) == 0){ // 当有重复的 ssid 时
            String EEPROM_Data_ssid = EEPROM_Data.substring(0, EEPROM_Data.indexOf("霖"));
            if (EEPROM_Data_ssid == ssid){ // 且重复的 ssid 相同
                for (int k = 0; k < password.length(); k++){
                    EEPROM.write(i*100 + ssid.length() + 3 + k, password[k]);
                }
                EEPROM.write(i*100 + data.length(), '\0'); // 添加结束符
                EEPROM.commit();
                return;
            }
        }
    }
    for (int i = 0; i < 5; i++){  // 遍历 5 个wifi信息块查找标记符位置
        char marker = EEPROM.read(i*100 + 99);
        if (marker == '1' && i != 4){  // 找到标记符位置
            EEPROM.write(i*100 + 99, '0');  // 删除旧的标记符
            for (int j = 0; j < data.length(); j++) {  // 写入 ssid 到 EEPROM
                EEPROM.write((i+1)*100 + j, data[j]);
            }
            EEPROM.write((i+1)*100 + data.length(), '\0');  // 写入结束字符
            EEPROM.write((i+1)*100 + 99, '1');  // 更新标记符位置
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

/*   测试专用   */
void readEEPROMData() {  // 读取 EEPROM 数据 
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
    Serial.println("handleRoot");
    File htmlFile = SPIFFS.open("/NetworkConfig.html", "r");
    if (!htmlFile) {
        server.send(404, "text/plain", "File Not Found");
    }
    server.streamFile(htmlFile, "text/html");
    htmlFile.close();
}
void handleConnectWifi() {
    Serial.println("handleConnectWifi");
    if (server.hasArg("plain")){ // IF 接收到 plain 参数
        String data = server.arg("plain");
        Serial.println(data);
        int separatorIndex = data.indexOf("霖");  // 无懈可击的分隔符
        if (separatorIndex != -1){  // IF 数据有效(查询到分隔符)
            String ssid = data.substring(0, separatorIndex);
            String password = data.substring(separatorIndex + 3);
            Serial.print("SSID:");
            Serial.println(ssid);
            Serial.print("Password:");
            Serial.println(password);
            if (WiFi.getMode() == WIFI_MODE_STA) {  // 如果当前模式是 STA 模式 则在超时后切换到 AP 模式
                
                Serial.println("在STA模式下连接超时，正在切换到 AP 模式");
                server.send(400, "text/plain", "当前为STA模式，无法连接WIFI，请切换到AP模式！");     // <<----- BUG 错误点 
                /*
                    发现错误，在STA模式下，超时后没有吧message发送给客户端，导致客户端报错，客户端未能处理超时信息，但是进入了AP模式
                    应当考虑是否有必要设置此功能！
                */
                // delay(1000);
                // WiFi.mode(WIFI_MODE_AP);
                // WiFi.softAP(AP_ssid, AP_password);
                return;
            } else {
                WiFi.begin(ssid, password);
                unsigned long startTime = millis();
                while (millis() - startTime < 5000 ) { // 等待连接WIFI 直到连接成功 超时后退出循环
                    uint8_t wifiStatus = WiFi.status(); // 获取 WiFi 连接状态
                    if (wifiStatus == WL_CONNECTED){  // IF 连接成功
                        Serial.println("Connect Wifi Success");
                        saveWiFiData(ssid, password);  // 保存 WiFi 信息到 EEPROM
                        readEEPROMData();
                        String ip = WiFi.localIP().toString();  // 获取 IP 地址
                        Serial.println("IP: " + ip);
                        server.send(200, "text/plain", "WiFi连接成功! 设备已进入STA模式，IP: " + ip);
                        delay(100);
                        WiFi.softAPdisconnect(true);  // 断开 AP 模式
                        server.client().stop();  // 关闭客户端连接
                        WiFi.mode(WIFI_MODE_STA);  // 切换回 STA 模式
                        Serial.println("WiFi mode: " + String(WiFi.getMode()));
                        return;
                    } else if(wifiStatus == WL_CONNECT_FAILED){
                        server.send(400, "text/plain", "WiFi连接失败! 请检查密码后重试！");
                        return;
                    } 
                    Serial.println("正在连接WIFI...");
                    delay(500);
                }
                Serial.println("Connect Wifi Failed: Timeout     WiFi mode: " + String(WiFi.getMode()));
            }
            WiFiClient client = server.client();
            if (client.connected() && client){
                server.send(400, "text/plain", "WiFi连接超时! 请检查密码后重试！");
            }else{
                Serial.println("Client not connected");
            }
            return;
        } else{ // IF 数据无效(没有查询到分隔符)
            Serial.println("Invalid data");
            server.send(400, "text/plain", "[ERROR]数据无效无分隔符！");
            return;
        }
    }else{ // IF 没有接收到 plain 参数
        server.send(400, "text/plain", "[ERROR]没有接收到 plain 参数！");
        return;
    }
}

String GetWifiListjson() {
    Serial.println("GetWifiListjson");
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

uint8_t parseEEPROMData() {  // 解析 EEPROM 数据 并保存到 ssidsFromEEPROM 和 passwordsFromEEPROM 数组中 并返回 ssidsFromEEPROM 数组的长度
    ssidsFromEEPROM[5] = {""};
    passwordsFromEEPROM[5] = {""};
    uint8_t ssidsFromEEPROM_count = 0;
    char ch;
    for (int i = 0; i < 5; i++){  // 遍历 5 个wifi信息块
        String data = "";
        for (int j = 0; j < 100; j++) {  // 读取 100 字节数据
            ch = EEPROM.read(i*100 + j);
            if(ch == '\0') break;
            data += ch;
        }
        if (data.length() > 0){  // IF 块内有数据
            int separatorIndex = data.indexOf("霖");  // 无懈可击的分隔符
            if (separatorIndex != -1){  // IF 数据有效(查询到分隔符)
                String ssid = data.substring(0, separatorIndex);    // 截取 ssid
                String password = data.substring(separatorIndex + 3);   // 截取密码
                for (int k = 0; k < 5; k++){    // 遍历 5 个wifi信息块查找空位
                    if (ssidsFromEEPROM[k].isEmpty()){  // 保存账号密码
                        ssidsFromEEPROM[k] = ssid;
                        passwordsFromEEPROM[k] = password;
                        ssidsFromEEPROM_count++;
                        break;
                    }
                }
            }
        }
    }
    return ssidsFromEEPROM_count;
}

uint8_t TrytoConnectWifi() {
    uint8_t ssidsFromEEPROM_count = parseEEPROMData();
    if(ssidsFromEEPROM_count == 0) return 0;
    GetWifiListjson();
    for (int i = 0; i < ssidFromEEPROM_count; i++) {  // 遍历 EEPROM 保存的 WiFi 信息
        for (int j = 0; j < wifiList.count; j++) {  // 遍历扫描到的 WiFi 信息
            if (wifiList.info[j].ssid == ssidsFromEEPROM[i]) {
                WiFi.begin(wifiList.info[j].ssid, passwordsFromEEPROM[i]);
                unsigned long startTime = millis();
                while (millis() - startTime < 5000 ) { // 等待连接WIFI 直到连接成功 超时后退出循环
                    uint8_t wifiStatus = WiFi.status(); // 获取 WiFi 连接状态
                    if (wifiStatus == WL_CONNECTED){  // IF 连接成功
                        return 1;
                    }
                }
                
            }
        }
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
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAP(AP_ssid, AP_password);
    server.on("/", handleRoot); 
    server.on("/connect_wifi", handleConnectWifi);
    server.on("/get_wifi_data", []() {server.send(200, "application/json", GetWifiListjson());});
    server.begin();
    readEEPROMData();
}

void loop() {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
    server.handleClient();
    
}
