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

    const unsigned char* icons[] = {warning_icon, charging_icon, wifi_icon, no_wifi_icon, battery_icon, ring_icon, no_ring_icon};
    for (int i = 0; i < sizeof(icons) / sizeof(char*); i++){
        int xVal = 225+(i*20);
        display.setPartialWindow(xVal, 12, 15, 15);
        display.firstPage();
        do{
            display.fillRect(xVal, 12, 15, 15, GxEPD_WHITE); // 清除界面
            display.drawBitmap(xVal, 12, icons[i], 15, 15, GxEPD_BLACK); // 显示图标
        }while (display.nextPage());
    }
    delay(1000);

    for (int i = 0; i < 3; i++) // 颗粒物数值显示
    {
        uint8_t Yval = 15+i*45;
        display.setPartialWindow(60, Yval, BoxWidth, BoxHeight);
        display.firstPage();
        do{
            display.fillRect(60, Yval, BoxWidth, BoxHeight, GxEPD_WHITE); // 清除界面
            display.setTextColor(GxEPD_BLACK);
            display.setFont(&FreeSansOblique24pt7b);
            display.setCursor(60, Yval+BoxHeight-2);
            display.print("5210");
        }while (display.nextPage());
    }
    
    
}

void loop(){

}