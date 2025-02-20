#include <GxEPD2_2IC_BW.h>
#include <Interface.h>

GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=*/ 41,42,/*DC=*/ 3, /*RST=*/ 8, /*BUSY=*/ 10)); // GDEH042A03-A1

void setup()
{
    display.init();
    delay(5000);
    display.firstPage();
    do{
        // display.fillScreen(GxEPD_WHITE);
        display.drawXBitmap(0, 0, interface_1, 400, 300, GxEPD_BLACK);
    }while (display.nextPage());
}

void loop()
{
}