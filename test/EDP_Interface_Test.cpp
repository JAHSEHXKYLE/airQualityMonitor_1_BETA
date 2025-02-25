#include <GxEPD2_2IC_BW.h>
#include <Icons_Library.h>
#include <Fonts/FreeSansOblique18pt7b.h>
#include <Fonts/FreeSansOblique12pt7b.h>
#include "AllSensors_Library.h"

#define BUZZER_PIN 47
#define BoxWidth 110 // 数显盒子尺寸
#define BoxHeight 30
#define LittleBoxWidth 60
#define LittleBoxHeight 25

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 35,42,/*DC=*/ 36, /*RST=*/ 37, /*BUSY=*/ 45)); // GDEH042A03-A1

ALL_SENSORS all_sensors;

void refresh_display(char str[][10], int len);
void GetAllData(char array[][10]);

char str[19][10];

void setup(){
    int tones[] = {2100, 5210};
    pinMode(BUZZER_PIN, OUTPUT);
    tone(BUZZER_PIN, tones[1], 110);
    delay(210);
    tone(BUZZER_PIN, tones[0], 210);
    all_sensors.init_all_sensors();
    display.init();
    display.firstPage();
    do{
        // display.fillScreen(GxEPD_WHITE);
        display.drawXBitmap(0, 0, interface_1, 400, 300, GxEPD_BLACK); // 显示界面
    }while (display.nextPage());
    for (int i = 0; i < 10; i++){
        strcpy(str[i], "?");
    }
}

void loop(){
    
    unsigned long MainTime = millis();
    GetAllData(str);
    refresh_display(str, 10);
    MainTime = millis() - MainTime;
    Debug_Serial.printf("\n\n\nLoop Time : %d ms\n\n\n", MainTime);
}

void refresh_display(char str[][10], int len){

/*
    const unsigned char* icons[] = {warning_icon, charging_icon, wifi_icon, no_wifi_icon, battery_icon, ring_icon, no_ring_icon};
    for (int i = 0; i < sizeof(icons) / sizeof(icons[0]); i++){
        int xVal = 225+(i*20);
        display.setPartialWindow(xVal, 12, 15, 15);
        display.firstPage();
        do{
            display.fillRect(xVal, 12, 15, 15, GxEPD_WHITE); // 清除界面
            display.drawBitmap(xVal, 12, icons[i], 15, 15, GxEPD_BLACK); // 显示图标
        }while (display.nextPage());
    }
    delay(1000);
*/
    
    int Xval = 0, Yval = 0;
    unsigned long timeVal = micros();
    for (int i = 0; i < len; i++) // 传感器数值显示,显示顺序为PM1.0, PM2.5, PM10，CO2，eTVOC，eCO2，CH2O，Temp，Hum，pres
    {
        if (i < 3){
            Xval = 60;
            Yval = 20+i*45;
        } else if (i == 3) {Xval = 250; Yval = 55;
        } else if (i == 4) {Yval = 120;
        } else if (i == 5) {Yval = 180;
        } else if (i == 6) {Xval = 272; Yval = 245;
        } else if (i >= 7) {Xval = 20+(i-7)*80; Yval = 250;
        }
        if (i < 7){
            display.setPartialWindow(Xval, Yval, BoxWidth, BoxHeight);
        } else {
            display.setPartialWindow(Xval, Yval, LittleBoxWidth, LittleBoxHeight);
        }
    
        
        display.firstPage();
        do{ 
            if (i < 7){
                display.fillRect(Xval, Yval, BoxWidth, BoxHeight, GxEPD_WHITE); // 清除界面
                display.setTextColor(GxEPD_BLACK);
                display.setFont(&FreeSansOblique18pt7b);
                int len = strlen(str[i]);
                if (len > 4) len = 4;
                display.setCursor(Xval+(40-len*10), Yval+BoxHeight-2); //居中显示
                // display.setCursor(Xval, Yval+BoxHeight-2);
                display.print(str[i]);
            } else {
                display.fillRect(Xval, Yval, LittleBoxWidth, LittleBoxHeight, GxEPD_WHITE); // 清除界面
                display.setTextColor(GxEPD_BLACK);
                display.setFont(&FreeSansOblique12pt7b);
                int len = strlen(str[i]);
                if (len > 3) len = 3;
                //display.setCursor(Xval+(20-len*6), Yval+LittleBoxHeight-2); //居中显示
                display.setCursor(Xval, Yval+LittleBoxHeight-2);
                display.print(str[i]);
            }
        }while (display.nextPage());
    }
}

void GetAllData(char array[][10]) {
    float temp_val, humi_val;
    int BMP_temp, BMP_pres;
    int pms_datas[12];
    float co2_val;
    float ch2o_val;
    uint16_t eco2_val, tvoc_val;
    unsigned long get_data_time = millis();
    all_sensors.GetAHT10Data(&temp_val, &humi_val);
    all_sensors.GetBMP280Data(&BMP_temp, &BMP_pres);
    all_sensors.GetPMS7003IData(pms_datas);
    all_sensors.GetSC8Data(&co2_val);
    all_sensors.GetWZSData(&ch2o_val);
    all_sensors.GetCCS811Data(&eco2_val, &tvoc_val);
    get_data_time = millis() - get_data_time;
    Debug_Serial.printf("\n\n\nGet Data Time : %d ms\n\n\n", get_data_time);
    sprintf(array[0], "%d", pms_datas[3]);   //大气环境下PM1.0 ug/m3
    sprintf(array[1], "%d", pms_datas[4]);   //大气环境下PM2.5 ug/m3
    sprintf(array[2], "%d", pms_datas[5]);   //大气环境下PM10 ug/m3
    sprintf(array[3], "%.1f", co2_val);
    sprintf(array[4], "%d", tvoc_val);
    sprintf(array[5], "%d", eco2_val);
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

    String str[] = {"PM1.0", "PM2.5", "PM10", "CO2", "TVOC", "eCO2", "CH2O", "温度", "湿度", "气压",  "PM1.0", "PM2.5", "PM10", "0.3μm颗粒物", "0.5μm颗粒物", "1.0μm颗粒物", "2.5μm颗粒物", "5.0μm颗粒物", "10.0μm颗粒物"};

    for (int i = 0; i < 19; i++) {
        if (i == 10) Debug_Serial.printf("\n\nOther Data:\n");
        Debug_Serial.print(str[i]);
        Debug_Serial.print(" : ");
        Debug_Serial.println(array[i]);
    }
    
}