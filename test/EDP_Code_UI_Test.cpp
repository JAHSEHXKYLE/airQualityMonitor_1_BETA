#include <GxEPD2_2IC_BW.h>
#include <Icons_Library.h>
#include <Arduino.h>

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 13,48,/*DC=*/ 14, /*RST=*/ 21, /*BUSY=*/ 47)); // GDEH042A03-A1

void setup() {
    display.init();
    display.setRotation(0); // 设置旋转方向
    display.firstPage();
    do{
        display.fillScreen(GxEPD_WHITE);
        display.drawGrayscaleBitmap(0, 0, gImage_1, 400, 300); // 显示界面
    }while (display.nextPage());
}

void loop() {

}