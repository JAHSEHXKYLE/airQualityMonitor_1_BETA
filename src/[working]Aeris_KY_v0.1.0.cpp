/*  
    Aeris_KY_v0.1.0cpp

    static const uint8_t SDA = 2;
    static const uint8_t SCL = 1;

    static const uint8_t SS = 37;
    static const uint8_t MOSI = 11;
    static const uint8_t MISO = 38;
    static const uint8_t SCK = 12;
*/
#include <WiFi.h>
#include <time.h>
#include <WebServer.h>
#include <EEPROM.h>  //存储wifi信息
#include <SPIFFS.h>  //存储html文件
#include <GxEPD2_2IC_BW.h> // 2.7寸黑白 GDEW027W3
#include <Icons_Library.h> // 图标库
#include <FreeRTOS.h>
#include <semphr.h>   
#include "AllSensors_Library.h" // 传感器库
#include <Fonts/FreeSansOblique18pt7b.h>
#include <Fonts/FreeSansOblique12pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <pins_arduino.h>

#include "ai_vox_engine.h"
#include "ai_vox_observer.h"
#include "i2s_std_audio_input_device.h"
#include "i2s_std_audio_output_device.h"

#define EEPROM_SIZE 500  // EEPROM 最大容量  每个数据由32字节的 ssid、63字节的密码、3个间隔字符、一个标记字符和一个结束字符组成 每个数据共占用100字节
                            // 所以 EEPROM 最大容量为 500/100 = 5 个数据

#define MAX_WIFI_COUNT 24 // web端显示的最大保存的 WiFi 数量 （最大不超过255个）

#define BoxWidth 110 // 数显盒子尺寸
#define BoxHeight 30
#define LittleBoxWidth 65
#define LittleBoxHeight 25

unsigned int Background_Color = GxEPD_WHITE;
unsigned int Font_Color = GxEPD_BLACK;

const char *AP_ssid = "AirMonitor·_BETA";
const char *AP_password = "12345678";
String ssidsFromEEPROM[5];   // 存放从 EEPROM 读取的 ssid 信息
String passwordsFromEEPROM[5];

struct WifiInfo {   // 存放从 WiFi.scan() 读取的 wifi 信息
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

struct SensorsData {    // 存放所有的传感器数据
    float temperature;
    float humidity;
    float pressure;
    float co2;
    float tvoc;
};

TaskHandle_t serverTaskHandle; // 服务器任务句柄
TaskHandle_t AerisAiTaskHandle; // Ai 任务句柄
SemaphoreHandle_t Aeris_Ai_mutex;  // 创建互斥锁

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 13,48,/*DC=*/ 14, /*RST=*/ 21, /*BUSY=*/ 47));
SensorsData Sensors;
WifiList wifiList;
WebServer server(80);
ALL_SENSORS all_sensors;

String GetWifiListjson();
void handleRoot();
void handleConnectWifi();
void handleGetSensorsData();
void Get_All_Sensors_Data(char array[][10]);

namespace {

    constexpr gpio_num_t kMicPinBclk = GPIO_NUM_5;
    constexpr gpio_num_t kMicPinWs = GPIO_NUM_4;
    constexpr gpio_num_t kMicPinDin = GPIO_NUM_6;

    constexpr gpio_num_t kSpeakerPinBclk = GPIO_NUM_15;
    constexpr gpio_num_t kSpeakerPinWs = GPIO_NUM_16;
    constexpr gpio_num_t kSpeakerPinDout = GPIO_NUM_7;

    constexpr gpio_num_t kTriggerPin = GPIO_NUM_8;

    auto g_observer = std::make_shared<ai_vox::Observer>();
    std::shared_ptr<ai_vox::iot::Entity> g_speaker_iot_entity;
    std::shared_ptr<ai_vox::iot::Entity> indoor_air_quality_iot_entity;
    auto g_audio_output_device = std::make_shared<ai_vox::I2sStdAudioOutputDevice>(kSpeakerPinBclk, kSpeakerPinWs, kSpeakerPinDout);

    void InitIot(char sensor_data_char[][10]) {
        printf("InitIot\n");
        auto& ai_vox_engine = ai_vox::Engine::GetInstance();

        std::vector<ai_vox::iot::Property> indoor_air_quality_properties({
            {
                "pm1.0",
                "室内PM1.0浓度",
                ai_vox::iot::ValueType::kString
            },
            {
                "pm2.5",
                "室内PM2.5浓度",
                ai_vox::iot::ValueType::kString
            },
            {
                "pm10.0",
                "室内PM10.0浓度",
                ai_vox::iot::ValueType::kString
            },
            {
                "TVOC",
                "室内TVOC浓度 单位ppb",
                ai_vox::iot::ValueType::kString
            },
            {
                "CO2",
                "室内二氧化碳浓度 单位ppm",
                ai_vox::iot::ValueType::kString
            },
            {
                "CH2O",
                "室内甲醛浓度 单位ppm",
                ai_vox::iot::ValueType::kString
            },
            {
                "humidity",
                "室内湿度 单位%RH",
                ai_vox::iot::ValueType::kString
            },
            {
                "temperature",
                "室内温度",
                ai_vox::iot::ValueType::kString
            },
            {
                "pressure",
                "室内气压",
                ai_vox::iot::ValueType::kString
            },
        });

        std::vector<ai_vox::iot::Function> indoor_air_quality_functions({
            {"GetIndoorAirQuality",
                "获取当前室内空气质量",
                {
                }
            },
        });

        indoor_air_quality_iot_entity = std::make_shared<ai_vox::iot::Entity>(
            "IndoorAirQuality",
            "室内空气质量检测器",
            std::move(indoor_air_quality_properties),
            std::move(indoor_air_quality_functions)
        );

        indoor_air_quality_iot_entity->UpdateState("pm1.0", sensor_data_char[0]);
        indoor_air_quality_iot_entity->UpdateState("pm2.5", sensor_data_char[1]);
        indoor_air_quality_iot_entity->UpdateState("pm10.0", sensor_data_char[2]);
        indoor_air_quality_iot_entity->UpdateState("TVOC", sensor_data_char[4]);
        indoor_air_quality_iot_entity->UpdateState("CO2", sensor_data_char[5]);
        indoor_air_quality_iot_entity->UpdateState("CH2O", sensor_data_char[6]);
        indoor_air_quality_iot_entity->UpdateState("humidity", sensor_data_char[8]);
        indoor_air_quality_iot_entity->UpdateState("temperature", sensor_data_char[7]);
        indoor_air_quality_iot_entity->UpdateState("pressure", sensor_data_char[9]);

        ai_vox_engine.RegisterIotEntity(indoor_air_quality_iot_entity);

        std::vector<ai_vox::iot::Property> speaker_properties({
            {
                "volume",
                "当前音量值",
                ai_vox::iot::ValueType::kNumber 
            },
        });

        std::vector<ai_vox::iot::Function> speaker_functions({
            {"SetVolume",
            "设置音量",
            {
                {
                    "volume",
                    "0到100之间的整数",
                    ai_vox::iot::ValueType::kNumber,
                    true
                },
            }},
        });

        g_speaker_iot_entity = std::make_shared<ai_vox::iot::Entity>(
            "Speaker",
            "扬声器",
            std::move(speaker_properties),
            std::move(speaker_functions)
        );

        g_speaker_iot_entity->UpdateState("volume", g_audio_output_device->volume());

        ai_vox_engine.RegisterIotEntity(g_speaker_iot_entity);

    }
}  // namespace

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

void clearEEPROMData() {  // 清空 EEPROM 数据
    for (int i = 0; i < 5; i++){  // 遍历 5 个wifi信息块
        for (int j = 0; j < 100; j++) {  // 写入 0 到 100 字节
            EEPROM.write(i*100 + j, '-');
        }
        EEPROM.commit();
    }
}

void handleRoot() {
    Serial.print("handleRoot    ");
    File htmlFile;
    if (WiFi.getMode() == WIFI_MODE_STA){ // 判断并更换主页文件
        Serial.println("STA mode");
        htmlFile = SPIFFS.open("/Main.html", "r");
    } else {
        Serial.println("AP mode");
        htmlFile = SPIFFS.open("/NetworkConfig.html", "r");
    }
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
                server.send(400, "text/plain", "当前为STA模式无法连接WIFI，再次点击“确定”将切换到AP模式尝试连接！");     
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

/* 传感器返回数据 全局变量 */
float AHT_temp = 0, AHT_hum = 0;
int BMP280_temp = 0, BMP280_press = 0, Tvoc_data = 0;
int PMS_data[12] = {0};
String PMS_data_name[12] = {"pm1_0", "pm2_5", "pm10_0", "pm1_0_atm", "pm2_5_atm", "pm10_0_atm", "n0_3", "n0_5", "n1_0", "n2_5", "n5_0", "n10_0"};
float CO2_data = 0, CH2O_data = 0;
long UV_data = 0;

char sensor_data_char[10][10] = {};

unsigned long last_data_save_time = millis();

void handleGetSensorsData() {
    Serial.println("handleGetSensorsData");
    String json = "{";
    json += "\"tvoc\":" + String(Tvoc_data) + ",";
    json += "\"ch2o\":" + String(CH2O_data, 3) + ",";
    for (int i = 0; i < 12; i++){
        json += "\"" + PMS_data_name[i] + "\":" + String(PMS_data[i]) + ",";
    }
    
    json += "\"co2\":" + String(CO2_data, 1) + ",";
    json += "\"temperature\":" + String(AHT_temp, 2) + ","; //温度校准 JASHEHKYLE
    json += "\"humidity\":" + String(AHT_hum, 2) + ",";
    json += "\"pressure\":" + String(BMP280_press);
    json += "}";
    Serial.println(json);
    server.send(200, "application/json", json);
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
        Serial.printf("\n\nGetWifiListjson: %s\n\n", json.c_str());
        return json;
    }
}

uint8_t parseEEPROMData() {  // 解析 EEPROM 数据 并保存到 ssidsFromEEPROM 和 passwordsFromEEPROM 数组中 并返回 ssidsFromEEPROM 中wifi的数量
    for (int k = 0; k < 5; k++) {
        ssidsFromEEPROM[k] = "";
        passwordsFromEEPROM[k] = "";
    }
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

uint8_t TrytoConnectWifi() {  // 尝试连接 EEPROM 保存的 WiFi 信息 返回 0 表示EEPROM中没有保存的 WiFi 信息 ;1 表示连接成功; 2 表示连接失败; 3 表示未找到 EEPROM 保存的 WiFi 信息
    uint8_t ssidsFromEEPROM_count = parseEEPROMData();
    if(ssidsFromEEPROM_count == 0) return 0;
    GetWifiListjson();
    for (int i = 0; i < ssidsFromEEPROM_count; i++) {  // 遍历 EEPROM 保存的 WiFi 信息
        for (int j = 0; j < wifiList.count; j++) {  // 遍历扫描到的 WiFi 信息
            if (wifiList.info[j].ssid == ssidsFromEEPROM[i]) {
                WiFi.begin(wifiList.info[j].ssid, passwordsFromEEPROM[i]);
                unsigned long startTime = millis();
                while (millis() - startTime < 5000 ) { // 等待连接WIFI 直到连接成功 超时后退出循环
                    uint8_t wifiStatus = WiFi.status(); // 获取 WiFi 连接状态
                    if (wifiStatus == WL_CONNECTED){  // IF 连接成功
                        Serial.printf("wifi自动连接成功   ");
                        Serial.println("IP: " + WiFi.localIP().toString());
                        return 1; // 连接成功
                    }
                    Serial.println("自动连接WIFI中...");
                    delay(500);
                }
                Serial.println("自动连接WIFI失败: Timeout");
                return 2; // 连接失败
            }
        }
    }  
    return 3;  // 未找到 EEPROM 保存的 WiFi 信息
}

struct sensor_threshold {
    uint16_t pm1_0_th  = 35;
    uint16_t pm2_5_th = 50;
    uint16_t pm10_0_th = 100;
    uint16_t UV_th = 795;
    uint16_t TVOC_th = 600;
    float CO2_th = 1075;
    float CH2O_th = 0.080f;
} sensor_threshold;

void refresh_display(char str[][10], int len){
    
    display.setPartialWindow(225, 12, 155, 15);
    if (WiFi.getMode() == WIFI_MODE_STA && WiFi.localIP().toString() != "0.0.0.0"){ // 显示设备IP地址
        display.firstPage();
        do{
            display.fillRect(220, 12, 160, 15, Background_Color); // 清除界面
            display.setTextColor(Font_Color);
            display.setFont(&FreeSansBoldOblique9pt7b);
            display.setCursor(220, 25); //居中显示
            display.print(" IP" + WiFi.localIP().toString());
        }while (display.nextPage());
    }else if (WiFi.getMode() == WIFI_MODE_STA && WiFi.localIP().toString()  == "0.0.0.0"){
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.softAP(AP_ssid, AP_password);
    }else{
        display.firstPage();
        do{
            display.fillRect(220, 12, 160, 15, Background_Color); // 清除界面
            display.setTextColor(Font_Color);
            display.setFont(&FreeSansBoldOblique9pt7b);
            display.setCursor(220, 25); //居中显示
            display.print("WiFi Disconnected​");
        }while (display.nextPage());
        
    }
    int val_position[][2] = {
        {60, 20}, // PM1.0
        {60, 65}, // PM2.5
        {60, 110}, // PM10
        {240, 55}, // UV
        {260, 120}, // TVOC
        {265, 175}, // CO2
        {272, 245}, // CH2O
        {20, 250}, // temp
        {100, 250}, // hum
        {180, 250} // pres
    };

    int Xval = 0, Yval = 0;
    unsigned long timeVal = micros();
    for (int i = 0; i < len; i++) // 传感器数值显示,显示顺序为PM1.0, PM2.5, PM10，UV，TVOC，CO2，CH2O，Temp，Hum，pres
    {
        Xval = val_position[i][0];
        Yval = val_position[i][1];
        if (i < 7){
            display.setPartialWindow(Xval, Yval, BoxWidth, BoxHeight);
        } else {
            display.setPartialWindow(Xval, Yval, LittleBoxWidth, LittleBoxHeight);
        }
    
        switch (i)
        {
        case 0: // PM1.0
            if (PMS_data[3] > sensor_threshold.pm1_0_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        case 1: // PM2.5
            if (PMS_data[4] > sensor_threshold.pm2_5_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        case 2: // PM10
            if (PMS_data[5] > sensor_threshold.pm10_0_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        case 3: // UV
            if (UV_data > sensor_threshold.UV_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        case 4: // TVOC
            if (Tvoc_data > sensor_threshold.TVOC_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        case 5: // CO2
            if (CO2_data > sensor_threshold.CO2_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        case 6: // CH2O
            if (CH2O_data > sensor_threshold.CH2O_th) {
                Background_Color = GxEPD_BLACK;
                Font_Color = GxEPD_WHITE;
            }else {
                Background_Color = GxEPD_WHITE;
                Font_Color = GxEPD_BLACK;
            }
            break;
        default:
            Background_Color = GxEPD_WHITE;
            Font_Color = GxEPD_BLACK;
            break;
        }

        display.firstPage();
        do{ 
            if (i < 7){
                display.fillRect(Xval, Yval, BoxWidth, BoxHeight, Background_Color); // 清除界面
                display.setTextColor(Font_Color);
                display.setFont(&FreeSansOblique18pt7b);
                int len = strlen(str[i]);
                display.setCursor(Xval+(55-len*9), Yval+BoxHeight-2); //居中显示3
                display.print(str[i]);
            } else {
                display.fillRect(Xval, Yval, LittleBoxWidth, LittleBoxHeight, Background_Color); // 清除界面
                display.setTextColor(Font_Color);
                display.setFont(&FreeSansOblique12pt7b);
                int len = strlen(str[i]);
                display.setCursor(Xval+(30-len*6), Yval+LittleBoxHeight-2); //居中显示
                display.print(str[i]);
            }
        }while (display.nextPage());
    }
}

void Get_All_Sensors_Data(char array[][10]) {
    Wire.end();
    Wire.begin(SDA_PIN, SCL_PIN);
    all_sensors.GetAHT10Data(&AHT_temp, &AHT_hum);
    AHT_temp = AHT_temp - 8;
    all_sensors.GetBMP280Data(&BMP280_temp, &BMP280_press);
    all_sensors.GetPMS7003IData(PMS_data);
    all_sensors.GetSC8Data(&CO2_data);
    all_sensors.GetWZSData(&CH2O_data);
    all_sensors.GetUVData(&UV_data);
    Wire.end();
    Wire.begin(SCL_PIN, SDA_PIN); // 弥补硬件BUG，重新初始化I2C
    all_sensors.GetAGS10Data(&Tvoc_data);
    sprintf(array[0], "%d", PMS_data[3]);   //大气环境下PM1.0 ug/m3
    sprintf(array[1], "%d", PMS_data[4]);   //大气环境下PM2.5 ug/m3
    sprintf(array[2], "%d", PMS_data[5]);   //大气环境下PM10 ug/m3
    sprintf(array[3], "%d", UV_data);
    sprintf(array[4], "%d", Tvoc_data);
    sprintf(array[5], "%.1f", CO2_data);
    sprintf(array[6], "%.3f", CH2O_data);
    sprintf(array[7], "%.2f", AHT_temp);
    sprintf(array[8], "%.2f", AHT_hum);
    sprintf(array[9], "%d", BMP280_press);
}

void AerisAiTask(void *pvParameters) {
    while (1)
    {
        if(xSemaphoreTake(Aeris_Ai_mutex, portMAX_DELAY)){
            Get_All_Sensors_Data(sensor_data_char);
            refresh_display(sensor_data_char, 10);
            xSemaphoreGive(Aeris_Ai_mutex);
        }
        delay(1);
    }  
}

void serverTask(void *pvParameters) {
    while (1){
        server.handleClient();
        delay(1);
    }
}

void setup() {
    Debug_Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    all_sensors.init_all_sensors();
    all_sensors.setTestMode(false);
    display.init();
    display.firstPage();
    do{
        display.fillScreen(GxEPD_WHITE);
        display.drawXBitmap(0, 0, interface_1, 400, 300, GxEPD_BLACK); // 显示界面
        delay(500);
    }while (display.nextPage());

    if (!SPIFFS.begin()) {
        Debug_Serial.println("SPIFFS Mount Failed");
        SPIFFS.format();
        Debug_Serial.println("SPIFFS format");
        return;
    }
    if (!EEPROM.begin(EEPROM_SIZE)){
        Serial.println("EEPROM Begin Failed");
        return;
    }
    //clearEEPROMData();
    uint8_t StatusAfterTrytoConnectWifi = TrytoConnectWifi();
    if (StatusAfterTrytoConnectWifi == 1){  // IF 连接成功
        Serial.println("Connect Wifi Success");
    } else {
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.softAP(AP_ssid, AP_password);
    }
    server.on("/", handleRoot); 
    server.on("/connect_wifi", handleConnectWifi);
    server.on("/get_wifi_data", []() {server.send(200, "application/json", GetWifiListjson());});
    server.on("/get_sensors_data", handleGetSensorsData);

    server.begin();
    readEEPROMData();

    Aeris_Ai_mutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        AerisAiTask,     // 任务函数
        "AerisAiTask",   // 任务名称
        4096,           // 堆栈大小（建议≥4096字节）
        NULL,           // 参数（无）
        1,              // 优先级（1-24，越高优先级越高）
        &AerisAiTaskHandle, 
        0               // 核心编号（0或1）
    );;
    xTaskCreatePinnedToCore(
        serverTask,     // 任务函数
        "ServerTask",   // 任务名称
        4096,           // 堆栈大小（建议≥4096字节）
        NULL,           // 参数（无）
        1,              // 优先级（1-24，越高优先级越高）
        &serverTaskHandle, 
        0               // 核心编号（0或1）
    );
}

void loop() {
    
    if (WiFi.getMode() == WIFI_MODE_STA && WiFi.localIP().toString() != "0.0.0.0") {
            if(xSemaphoreTake(Aeris_Ai_mutex, portMAX_DELAY)){
                InitIot(sensor_data_char);
                xSemaphoreGive(Aeris_Ai_mutex);
            }  

            auto audio_input_device = std::make_shared<ai_vox::I2sStdAudioInputDevice>(kMicPinBclk, kMicPinWs, kMicPinDin);

            auto& ai_vox_engine = ai_vox::Engine::GetInstance();
            ai_vox_engine.SetObserver(g_observer);
            ai_vox_engine.SetTrigger(kTriggerPin);
            ai_vox_engine.SetOtaUrl("https://api.tenclass.net/xiaozhi/ota/");
            ai_vox_engine.ConfigWebsocket("wss://api.tenclass.net/xiaozhi/v1/",
                                            {
                                                {"Authorization", "Bearer test-token"},
                                            });
            ai_vox_engine.Start(audio_input_device, g_audio_output_device);
            printf("AI Vox engine started\n");

            Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
            Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());
            Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
            while (1) {
                const auto events = g_observer->PopEvents();
                for (auto& event : events) {
                    if (auto activation_event = std::get_if<ai_vox::Observer::ActivationEvent>(&event)) {
                        printf("activation code: %s, message: %s\n", activation_event->code.c_str(), activation_event->message.c_str());
                    } else if (auto state_changed_event = std::get_if<ai_vox::Observer::StateChangedEvent>(&event)) {
                        printf("state changed from %" PRIu8 " to %" PRIu8 "\n",
                        static_cast<uint8_t>(state_changed_event->old_state),
                        static_cast<uint8_t>(state_changed_event->new_state));
                        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
                        Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());
                        Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
                        switch (state_changed_event->new_state) {
                            case ai_vox::ChatState::kIdle: {
                                printf("Idle\n");
                                break;
                            }
                            case ai_vox::ChatState::kIniting: {
                                printf("Initing...\n");
                                break;
                            }
                            case ai_vox::ChatState::kStandby: {
                                printf("Standby\n");
                                break;
                            }
                            case ai_vox::ChatState::kConnecting: {
                                printf("Connecting...\n");
                                break;
                            }
                            case ai_vox::ChatState::kListening: {
                                printf("Listening...\n");
                                break;
                            }
                            case ai_vox::ChatState::kSpeaking: {
                                printf("Speaking...\n");
                                break;
                            }
                                default: {
                                break;
                            }
                        }
                    } else if (auto emotion_event = std::get_if<ai_vox::Observer::EmotionEvent>(&event)) {
                        printf("emotion: %s\n", emotion_event->emotion.c_str());
                    } else if (auto chat_message_event = std::get_if<ai_vox::Observer::ChatMessageEvent>(&event)) {
                        switch (chat_message_event->role) {
                            case ai_vox::ChatRole::kAssistant: {
                                printf("role: assistant, content: %s\n", chat_message_event->content.c_str());
                                break;
                            }
                            case ai_vox::ChatRole::kUser: {
                                printf("role: user, content: %s\n", chat_message_event->content.c_str());
                                break;
                            }
                        }
                    } else if (auto iot_message_event = std::get_if<ai_vox::Observer::IotMessageEvent>(&event)) {
                        printf("IOT message: %s, function: %s\n", iot_message_event->name.c_str(), iot_message_event->function.c_str());
                        for (const auto& [key, value] : iot_message_event->parameters) {
                            if (std::get_if<bool>(&value)) {
                                printf("key: %s, value: %s\n", key.c_str(), std::get<bool>(value) ? "true" : "false");
                            } else if (std::get_if<std::string>(&value)) {
                                printf("key: %s, value: %s\n", key.c_str(), std::get<std::string>(value).c_str());
                            } else if (std::get_if<int64_t>(&value)) {
                                printf("key: %s, value: %lld\n", key.c_str(), std::get<int64_t>(value));
                            }
                        }
                        if (iot_message_event->name == "IndoorAirQuality") {
                            if (iot_message_event->function == "GetIndoorAirQuality") {
                                printf("GetIndoorAirQuality\n");
                                if(xSemaphoreTake(Aeris_Ai_mutex, portMAX_DELAY)){
                                    indoor_air_quality_iot_entity->UpdateState("pm1.0", sensor_data_char[0]);
                                    indoor_air_quality_iot_entity->UpdateState("pm2.5", sensor_data_char[1]);
                                    indoor_air_quality_iot_entity->UpdateState("pm10.0", sensor_data_char[2]);
                                    indoor_air_quality_iot_entity->UpdateState("TVOC", sensor_data_char[4]);
                                    indoor_air_quality_iot_entity->UpdateState("CO2", sensor_data_char[5]);
                                    indoor_air_quality_iot_entity->UpdateState("CH2O", sensor_data_char[6]);
                                    indoor_air_quality_iot_entity->UpdateState("humidity", sensor_data_char[8]);
                                    indoor_air_quality_iot_entity->UpdateState("temperature", sensor_data_char[7]);
                                    indoor_air_quality_iot_entity->UpdateState("pressure", sensor_data_char[9]);
                                    xSemaphoreGive(Aeris_Ai_mutex);
                                }  
                            }
                        }
                        else if (iot_message_event->name == "Speaker") {
                            if (iot_message_event->function == "SetVolume") {
                                if (const auto it = iot_message_event->parameters.find("volume"); it != iot_message_event->parameters.end()) {
                                    auto volume = it->second;
                                    if (std::get_if<int64_t>(&volume)) {
                                        printf("Speaker volume: %lld\n", std::get<int64_t>(volume));
                                        g_audio_output_device->SetVolume(std::get<int64_t>(volume));
                                        g_speaker_iot_entity->UpdateState("volume", std::get<int64_t>(volume));  // Note: Must UpdateState after change the device state
                                    }
                                } 
                            }
                        }
                    }
                }
                taskYIELD();
            }
        }
}
