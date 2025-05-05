#include <GxEPD2_2IC_BW.h>
#include <Icons_Library.h>
#include <Fonts/FreeSansOblique18pt7b.h>
#include <Fonts/FreeSansOblique12pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include "AllSensors_Library.h"
#include <pins_arduino.h>

#define BUZZER_PIN 47
#define BoxWidth 110 // 数显盒子尺寸
#define BoxHeight 30
#define LittleBoxWidth 65
#define LittleBoxHeight 25

#define Background_Color GxEPD_WHITE
#define Font_Color GxEPD_BLACK
// #define Background_Color GxEPD_BLACK
// #define Font_Color GxEPD_WHITE 

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 13,48,/*DC=*/ 14, /*RST=*/ 21, /*BUSY=*/ 47)); // GDEH042A03-A1

ALL_SENSORS all_sensors;

void refresh_display(char str[][10], int len);
void GetAllData(char array[][10]);

char str[19][10];
int tvoc_val = 0;

void setup(){
    Wire.begin(SDA_PIN, SCL_PIN);
    all_sensors.init_all_sensors();
    all_sensors.setTestMode(false);
    display.init();
    display.firstPage();
    do{
        display.fillScreen(GxEPD_WHITE);
        display.drawXBitmap(0, 0, interface_1, 400, 300, GxEPD_BLACK); // 显示界面
    }while (display.nextPage());
}

void loop(){
    Wire.end();
    Wire.begin(SDA_PIN, SCL_PIN);

    unsigned long MainTime, refresh_time;
    MainTime = millis();
    GetAllData(str);
    refresh_time = millis();
    refresh_display(str, 10);
    refresh_time = millis() - refresh_time;
    MainTime = millis() - MainTime;
    Debug_Serial.printf("\n\n\nLoop Time : %d ms\n\n\n", MainTime);
    Debug_Serial.printf("Refresh Time : %d ms\n\n\n", refresh_time);
}

void refresh_display(char str[][10], int len){
    
    display.setPartialWindow(225, 12, 155, 15);
    display.firstPage();
    do{
        display.fillRect(220, 12, 160, 15, Background_Color); // 清除界面
        display.setTextColor(Font_Color);
        display.setFont(&FreeSansBoldOblique9pt7b);
        display.setCursor(220, 25); //居中显示
        display.print("WiFi Disconnected​ ");
    }while (display.nextPage());
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
    for (int i = 0; i < len; i++) // 传感器数值显示,显示顺序为PM1.0, PM2.5, PM10，CO2，eTVOC，eCO2，CH2O，Temp，Hum，pres
    {
        Xval = val_position[i][0];
        Yval = val_position[i][1];
        if (i < 7){
            display.setPartialWindow(Xval, Yval, BoxWidth, BoxHeight);
        } else {
            display.setPartialWindow(Xval, Yval, LittleBoxWidth, LittleBoxHeight);
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

void GetAllData(char array[][10]) {
    float temp_val, humi_val;
    int BMP_temp, BMP_pres, tvoc_val;
    int pms_datas[12] = {0};
    float co2_val;
    float ch2o_val;
    uint16_t eco2_val;
    long uv_val;
    unsigned long get_data_time = millis();
    all_sensors.GetAHT10Data(&temp_val, &humi_val);
    all_sensors.GetBMP280Data(&BMP_temp, &BMP_pres);
    all_sensors.GetPMS7003IData(pms_datas);
    all_sensors.GetSC8Data(&co2_val);
    all_sensors.GetWZSData(&ch2o_val);
    all_sensors.GetUVData(&uv_val);
    Wire.end();
    Wire.begin(1, 2);
    all_sensors.GetAGS10Data(&tvoc_val);
    //all_sensors.GetCCS811Data(&eco2_val, &tvoc_val);
    get_data_time = millis() - get_data_time;
    Debug_Serial.printf("\n\n\nGet Data Time : %d ms\n\n\n", get_data_time);
    sprintf(array[0], "%d", pms_datas[3]);   //大气环境下PM1.0 ug/m3
    sprintf(array[1], "%d", pms_datas[4]);   //大气环境下PM2.5 ug/m3
    sprintf(array[2], "%d", pms_datas[5]);   //大气环境下PM10 ug/m3
    sprintf(array[3], "%d", uv_val);
    sprintf(array[4], "%d", tvoc_val);
    sprintf(array[5], "%.1f", co2_val);
    sprintf(array[6], "%.3f", ch2o_val);
    sprintf(array[7], "%.2f", temp_val);
    sprintf(array[8], "%.2f", humi_val);
    sprintf(array[9], "%d", BMP_pres);

    sprintf(array[10], "%d", pms_datas[0]);  //PM1.0 ug/m3
    sprintf(array[11], "%d", pms_datas[1]);  //PM2.5 ug/m3
    sprintf(array[12], "%d", pms_datas[2]);  //PM10 ug/m3
    sprintf(array[13], "%d", pms_datas[6]);  //0.1L空气中直径大于0.3μm的颗粒物个数
    sprintf(array[14], "%d", pms_datas[7]);  //0.1L空气中直径大于0.5μm的颗粒物个数
    sprintf(array[15], "%d", pms_datas[8]);  //0.1L空气中直径大于1.0μm的颗粒物个数
    sprintf(array[16], "%d", pms_datas[9]);  //0.1L空气中直径大于2.5μm的颗粒物个数
    sprintf(array[17], "%d", pms_datas[10]); //0.1L空气中直径大于5.0μm的颗粒物个数
    sprintf(array[18], "%d", pms_datas[11]); //0.1L空气中直径大于10.0μm的颗粒物个数

    String str[] = {
        "PM1.0", "PM2.5", "PM10", 
        "UV", "TVOC", 
        "dCO2", 
        "CH2O", 
        "温度", "湿度", "气压",
        "PM1.0", "PM2.5", "PM10", "0.3μm颗粒物", "0.5μm颗粒物", "1.0μm颗粒物", "2.5μm颗粒物", "5.0μm颗粒物", "10.0μm颗粒物"
    };

    for (int i = 0; i < 19; i++) {
        if (i == 10) Debug_Serial.printf("\n\nOther Data:\n");
        Debug_Serial.print(str[i]);
        Debug_Serial.print(" : ");
        Debug_Serial.println(array[i]);
    }
    
}