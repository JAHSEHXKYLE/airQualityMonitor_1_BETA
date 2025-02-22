#include <GxEPD2_2IC_BW.h>
#include <Icons_Library.h>
#include <Fonts/FreeSansOblique24pt7b.h>

#define BoxWidth 105 // 数显盒子尺寸
#define BoxHeight 35

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 41,42,/*DC=*/ 3, /*RST=*/ 8, /*BUSY=*/ 10)); // GDEH042A03-A1

void setup(){
    display.init();

    display.firstPage();
    do{
        // display.fillScreen(GxEPD_WHITE);
        display.drawXBitmap(0, 0, interface_1, 400, 300, GxEPD_BLACK); // 显示界面
    }while (display.nextPage());
    delay(1000);
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
    char str[][10] = {"ERROR", "5210", "210", "5210", "5", "210", "0.05"};
    int Xval = 0, Yval = 0;
    for (int i = 0; i < 7; i++) // 颗粒物数值显示,显示顺序为PM1.0, PM2.5, PM10，CO2，eTVOC，eCO2，CH2O，Temp，Hum，pres
    {
        if (i < 3){
            Xval = 60;
            Yval = 15+i*45;
        } else if (i == 3) {Xval = 260; Yval = 50;
        } else if (i == 4) {Yval = 115;
        } else if (i == 5) {Yval = 170;
        } else if (i == 6) {Xval = 278; Yval = 245;
        }
        display.setPartialWindow(Xval, Yval, BoxWidth, BoxHeight);
        display.firstPage();
        do{
            display.fillRect(Xval, Yval, BoxWidth, BoxHeight, GxEPD_WHITE); // 清除界面
            display.setTextColor(GxEPD_BLACK);
            display.setFont(&FreeSansOblique24pt7b);
            display.setCursor(Xval, Yval+BoxHeight-2);
            display.print(str[i]);
        }while (display.nextPage());
    }
    
    
}

void loop(){

}