// This code is used to send data from the sensor to the web server.
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ConfigHtml.h>

#define BMP280_ADDRESS 0x76
#define AHT_adress 0x38

const char *ssid = "ESP32_S3_BATA";
const char *password = "12345678";
WebServer server(80);

float AHT_temp, AHT_hum;

unsigned long int temp_raw,pres_raw;
signed long int t_fine;
uint16_t dig_T1;
 int16_t dig_T2;
 int16_t dig_T3;
uint16_t dig_P1;
 int16_t dig_P2;
 int16_t dig_P3;
 int16_t dig_P4;
 int16_t dig_P5;
 int16_t dig_P6;
 int16_t dig_P7;
 int16_t dig_P8;
 int16_t dig_P9;
signed long int press_bmp280;
signed long int temp_bmp280;



uint16_t Tvoc_Data = 1111;
uint16_t Ch2o_Data = 2222;
uint16_t PM25_Data = 3333;
uint16_t CO2_Data = 4444;

void GetBMP280Data(unsigned long int *temp, unsigned long int *pres);
void I2c_Write_Reg(uint8_t I2c_address, uint8_t reg_address, uint8_t data);
uint8_t init_BMP280();
signed long int calibration_T(signed long int adc_T);
signed long int calibration_P(signed long int adc_P);

void GETAHT10Data(float* temp_val, float* humi_val);
void GetSensorData();

void setup() {
    Serial.begin(115200);
    Wire.begin();
    uint8_t ret = init_BMP280();
    WiFi.softAP(ssid, password);
    server.on("/", []() { server.send(200, "text/html", ConfigHtml::configHtml); });
    server.on("/GetSensorData", GetSensorData);
    server.begin();
}

void loop() {
    GETAHT10Data(&AHT_temp, &AHT_hum);
    GetBMP280Data(&temp_raw, &pres_raw);
    temp_bmp280 = calibration_T(temp_raw)/100;
    press_bmp280 = calibration_P(pres_raw)/100;
    Serial.print("Temperature: ");
    Serial.print(temp_bmp280);
    Serial.print("C, Pressure: ");
    Serial.print(press_bmp280);
    Serial.println("hPa");
    Serial.print("AHT Temperature: ");
    Serial.print(AHT_temp);
    Serial.print("C, Humidity: ");
    Serial.print(AHT_hum);
    Serial.println("%");
    server.handleClient();
    delay(10);
}

void GetSensorData() {
    String json = "{";
    json += "\"tvoc\":" + String(Tvoc_Data) + ",";
    json += "\"ch2o\":" + String(Ch2o_Data) + ",";
    json += "\"pm25\":" + String(PM25_Data) + ",";
    json += "\"co2\":" + String(CO2_Data) + ",";
    json += "\"temperature\":" + String(AHT_temp, 1) + ",";
    json += "\"humidity\":" + String(AHT_hum, 1) + ",";
    json += "\"pressure\":" + String(press_bmp280);
    json += "}";
    server.send(200, "application/json", json);
}

void GETAHT10Data(float* temp_val, float* humi_val){
    int data[6] = {0};
    Wire.beginTransmission(AHT_adress);
    Wire.write(0xAC); // 温度测量
    Wire.write(0x33);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(AHT_adress, 6);
    while (Wire.available()){
        for (int i = 0; i < 6; i++){
            data[i] = Wire.read();
        }
        Serial.print(data[0]);
        if (data[0] && 0x08 == 0){ // 检测校准使能位（为了兼容旧版本的AHT10）
            Wire.beginTransmission(AHT_adress);
            Wire.write(0xE1); // 初始化
            Wire.write(0x08);
            Wire.write(0x00);
            Wire.endTransmission();
            Serial.println("init AHT10");
        }
        else{
            Wire.beginTransmission(AHT_adress); // 读取数据
            Wire.write(0xAC); // 温度测量
            Wire.write(0x33);
            Wire.write(0x00);
            Wire.endTransmission();
            delayMicroseconds(75);
        }
    }
    int humi_raw = ((data[1] << 12) | (data[2] << 4)) | (data[3] >> 4);
    int temp_raw = ((data[3] & 0x0f) << 16) | (data[4] << 8) | data[5];
    *humi_val = (humi_raw / pow(2, 20)) * 100.0;
    *temp_val = (temp_raw / pow(2, 20)) * 200.0 - 50.0;
}

void I2c_Write_Reg(uint8_t I2c_address, uint8_t reg_address, uint8_t data) {
    Wire.beginTransmission(I2c_address);
    Wire.write(reg_address);
    Wire.write(data);
    delayMicroseconds(50);
    Wire.endTransmission();    
}

uint8_t init_BMP280() {
    uint8_t osrs_t = 2;             //Temperature oversampling x 2
    uint8_t osrs_p = 5;             //Pressure oversampling x 16
    uint8_t mode = 3;               //Normal mode
    uint8_t t_sb = 3;               //Tstandby 250ms
    uint8_t filter = 4;             //Filter 0.021 x ODR
    uint8_t spi3w_en = 0;           //3-wire SPI Disable
    
    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
    
    uint8_t data[24] = {0},i=0;
    Serial.begin(115200);
    Wire.begin();
    I2c_Write_Reg(BMP280_ADDRESS,0xE0,0xB6); //reset
    I2c_Write_Reg(BMP280_ADDRESS,0xF4,ctrl_meas_reg);
    I2c_Write_Reg(BMP280_ADDRESS,0xF5,config_reg);

    Wire.beginTransmission(BMP280_ADDRESS);
    Wire.write(0x88);
    delayMicroseconds(75); //等待 保证数据完整
    Wire.endTransmission(1);
    Wire.requestFrom(BMP280_ADDRESS,22);
    while(Wire.available()){
        data[i] = Wire.read();
        i++;
    }
    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11]<< 8) | data[10];
    dig_P4 = (data[13]<< 8) | data[12];
    dig_P5 = (data[15]<< 8) | data[14];
    dig_P6 = (data[17]<< 8) | data[16];
    dig_P7 = (data[19]<< 8) | data[18];
    dig_P8 = (data[21]<< 8) | data[20];
    dig_P9 = (data[23]<< 8) | data[22];
    return 1; //初始化成功
}

void GetBMP280Data(unsigned long int *temp, unsigned long int *pres)
{
    int i = 0;
    uint32_t data[6] = {0};
    Wire.beginTransmission(BMP280_ADDRESS);
    Wire.write(0xF7);
    Wire.endTransmission();
    Wire.requestFrom(BMP280_ADDRESS,6);
    while(Wire.available()){
        data[i] = Wire.read();
        i++;
    }
    *pres = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    *temp = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
}


signed long int calibration_T(signed long int adc_T)
{
    
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}

signed long int calibration_P(signed long int adc_P)
{
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0)
    {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000)
    {
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}
