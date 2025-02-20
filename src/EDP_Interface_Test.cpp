#include <GxEPD2_2IC_BW.h>
#include <Interface.h>

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 41,42,/*DC=*/ 3, /*RST=*/ 8, /*BUSY=*/ 10)); // GDEH042A03-A1

void setup()
{
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
}

void loop()
{

}