#include <EEPROM.h>

#define EEPROM_SIZE 512        // 定义 EEPROM 的大小
#define MAX_WIFI_ENTRIES 5     // 最大存储的 WiFi 数量
#define ENTRY_DELIMITER "|"    // 用于分隔 SSID 和密码的字符

// 保存 WiFi 数据到 EEPROM
void saveWiFiData(int index, const String &ssid, const String &password) {
    if (index < 0 || index >= MAX_WIFI_ENTRIES) {
        Serial.println("Invalid index!");
        return;
    }

    // 构造保存的字符串 "SSID|PASSWORD"
    String entry = ssid + ENTRY_DELIMITER + password;

    // 计算偏移地址（假设每个 WiFi 数据占用固定大小空间）
    int address = index * (EEPROM_SIZE / MAX_WIFI_ENTRIES);

    // 检查是否超出单个区段大小
    if (entry.length() >= EEPROM_SIZE / MAX_WIFI_ENTRIES) {
        Serial.println("Data too large for EEPROM section!");
        return;
    }

    // 写入字符串到 EEPROM
    for (int i = 0; i < entry.length(); ++i) {
        EEPROM.write(address + i, entry[i]);
    }
    EEPROM.write(address + entry.length(), '\0'); // 添加结束符
    EEPROM.commit();                              // 保存更改
    Serial.printf("WiFi data saved at index %d: %s\n", index, entry.c_str());
}

// 从 EEPROM 读取 WiFi 数据
bool readWiFiData(int index, String &ssid, String &password) {
    if (index < 0 || index >= MAX_WIFI_ENTRIES) {
        Serial.println("Invalid index!");
        return false;
    }

    // 计算偏移地址
    int address = index * (EEPROM_SIZE / MAX_WIFI_ENTRIES);

    // 读取字符串直到结束符
    String entry = "";
    char ch;
    for (int i = 0; i < EEPROM_SIZE / MAX_WIFI_ENTRIES; ++i) {
        ch = EEPROM.read(address + i);
        if (ch == '\0') break;
        entry += ch;
    }

    // 分割字符串到 SSID 和密码
    int delimiterIndex = entry.indexOf(ENTRY_DELIMITER);
    if (delimiterIndex == -1) {
        Serial.println("Invalid data format!");
        return false;
    }

    ssid = entry.substring(0, delimiterIndex);
    password = entry.substring(delimiterIndex + 1);

    Serial.printf("WiFi data read at index %d: SSID=%s, Password=%s\n", index, ssid.c_str(), password.c_str());
    return true;
}

void setup() {
    Serial.begin(115200);

    // 初始化 EEPROM
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("Failed to initialize EEPROM!");
        return;
    }

    // 示例：存储 WiFi 数据
    // saveWiFiData(0, "MyWiFi1", "password1");
    // saveWiFiData(1, "MyWiFi2", "password2");

    // 示例：读取并打印 WiFi 数据
    String ssid, password;
    if (readWiFiData(0, ssid, password)) {
        Serial.printf("Index 0: SSID=%s, Password=%s\n", ssid.c_str(), password.c_str());
    }
    if (readWiFiData(1, ssid, password)) {
        Serial.printf("Index 1: SSID=%s, Password=%s\n", ssid.c_str(), password.c_str());
    }
}

void loop() {
    // 可在此添加其他逻辑
}
