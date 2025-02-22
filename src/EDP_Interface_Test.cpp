#include <GxEPD2_2IC_BW.h>
#include <Icons_Library.h>
#include <Fonts/FreeSansOblique18pt7b.h>
#include <Fonts/FreeSansOblique12pt7b.h>

#define BoxWidth 110 // 数显盒子尺寸
#define BoxHeight 30
#define LittleBoxWidth 60
#define LittleBoxHeight 25
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
    char str[][10] = {"888", "88", "8", "8888", "8888", "8888", "0.0088", "-88.8", "99.9","1888"};
    int Xval = 0, Yval = 0;
    for (int i = 0; i < 10; i++) // 传感器数值显示,显示顺序为PM1.0, PM2.5, PM10，CO2，eTVOC，eCO2，CH2O，Temp，Hum，pres
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

void loop(){

}