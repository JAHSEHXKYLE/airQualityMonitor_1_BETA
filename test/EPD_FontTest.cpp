#include <GxEPD2_2IC_BW.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono24pt7b.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>

#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeMonoOblique12pt7b.h>
#include <Fonts/FreeMonoOblique18pt7b.h>
#include <Fonts/FreeMonoOblique24pt7b.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>
#include <Fonts/FreeSansBoldOblique18pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>

#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSansOblique12pt7b.h>
#include <Fonts/FreeSansOblique18pt7b.h>
#include <Fonts/FreeSansOblique24pt7b.h>

#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeSerif18pt7b.h>
#include <Fonts/FreeSerif24pt7b.h>

#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>

#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSerifBoldItalic12pt7b.h>
#include <Fonts/FreeSerifBoldItalic18pt7b.h>
#include <Fonts/FreeSerifBoldItalic24pt7b.h>

#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/FreeSerifItalic12pt7b.h>
#include <Fonts/FreeSerifItalic18pt7b.h>
#include <Fonts/FreeSerifItalic24pt7b.h>

#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>


GxEPD2_2IC_BW<GxEPD2_2IC_420_A03, GxEPD2_2IC_420_A03::HEIGHT> display(GxEPD2_2IC_420_A03(/*CS=5*/ 41,42,/*DC=*/ 3, /*RST=*/ 8, /*BUSY=*/ 10)); // GDEH042A03-A1

void showFont(const char name[], const GFXfont* f);
void drawFont(const char name[], const GFXfont* f);

void setup()
{
  Serial.begin(115200);
  Serial.printf("GxEPD2_2IC Demo \n\n\n");
  display.init(115200);
  showFont("FreeMono9pt7b", &FreeMono9pt7b);
  showFont("FreeMono12pt7b", &FreeMono12pt7b);
  showFont("FreeMono18pt7b", &FreeMono18pt7b);
  showFont("FreeMono24pt7b", &FreeMono24pt7b);

  showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
  showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
  showFont("FreeMonoBold18pt7b", &FreeMonoBold18pt7b);
  showFont("FreeMonoBold24pt7b", &FreeMonoBold24pt7b);

  showFont("FreeMonoBoldOblique9pt7b", &FreeMonoBoldOblique9pt7b);
  showFont("FreeMonoBoldOblique12pt7b", &FreeMonoBoldOblique12pt7b);
  showFont("FreeMonoBoldOblique18pt7b", &FreeMonoBoldOblique18pt7b);
  showFont("FreeMonoBoldOblique24pt7b", &FreeMonoBoldOblique24pt7b);

  showFont("FreeMonoOblique9pt7b", &FreeMonoOblique9pt7b);
  showFont("FreeMonoOblique12pt7b", &FreeMonoOblique12pt7b);
  showFont("FreeMonoOblique18pt7b", &FreeMonoOblique18pt7b);
  showFont("FreeMonoOblique24pt7b", &FreeMonoOblique24pt7b);

  showFont("FreeSans9pt7b", &FreeSans9pt7b);
  showFont("FreeSans12pt7b", &FreeSans12pt7b);
  showFont("FreeSans18pt7b", &FreeSans18pt7b);
  showFont("FreeSans24pt7b", &FreeSans24pt7b);

  showFont("FreeSansBold9pt7b", &FreeSansBold9pt7b);
  showFont("FreeSansBold12pt7b", &FreeSansBold12pt7b);
  showFont("FreeSansBold18pt7b", &FreeSansBold18pt7b);
  showFont("FreeSansBold24pt7b", &FreeSansBold24pt7b);

  showFont("FreeSansBoldOblique9pt7b", &FreeSansBoldOblique9pt7b);
  showFont("FreeSansBoldOblique12pt7b", &FreeSansBoldOblique12pt7b);
  showFont("FreeSansBoldOblique18pt7b", &FreeSansBoldOblique18pt7b);
  showFont("FreeSansBoldOblique24pt7b", &FreeSansBoldOblique24pt7b);

  showFont("FreeSansOblique9pt7b", &FreeSansOblique9pt7b);
  showFont("FreeSansOblique12pt7b", &FreeSansOblique12pt7b);
  showFont("FreeSansOblique18pt7b", &FreeSansOblique18pt7b);
  showFont("FreeSansOblique24pt7b", &FreeSansOblique24pt7b);

  showFont("FreeSerif9pt7b", &FreeSerif9pt7b);
  showFont("FreeSerif12pt7b", &FreeSerif12pt7b);
  showFont("FreeSerif18pt7b", &FreeSerif18pt7b);
  showFont("FreeSerif24pt7b", &FreeSerif24pt7b);

  showFont("FreeSerifBold9pt7b", &FreeSerifBold9pt7b);
  showFont("FreeSerifBold12pt7b", &FreeSerifBold12pt7b);
  showFont("FreeSerifBold18pt7b", &FreeSerifBold18pt7b);
  showFont("FreeSerifBold24pt7b", &FreeSerifBold24pt7b);

  showFont("FreeSerifBoldItalic9pt7b", &FreeSerifBoldItalic9pt7b);
  showFont("FreeSerifBoldItalic12pt7b", &FreeSerifBoldItalic12pt7b);
  showFont("FreeSerifBoldItalic18pt7b", &FreeSerifBoldItalic18pt7b);
  showFont("FreeSerifBoldItalic24pt7b", &FreeSerifBoldItalic24pt7b);

  showFont("FreeSerifItalic9pt7b", &FreeSerifItalic9pt7b);
  showFont("FreeSerifItalic12pt7b", &FreeSerifItalic12pt7b);
  showFont("FreeSerifItalic18pt7b", &FreeSerifItalic18pt7b);
  showFont("FreeSerifItalic24pt7b", &FreeSerifItalic24pt7b);

  showFont("Org_01", &Org_01);
  showFont("Picopixel", &Picopixel);
  showFont("Tiny3x3a2pt7b", &Tiny3x3a2pt7b);
  showFont("TomThumb", &TomThumb);
}

void loop()
{

}

void showFont(const char name[], const GFXfont* f)
{
    Serial.print(name);
    Serial.print("...");
  display.setFullWindow();
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    drawFont(name, f);
  }
  while (display.nextPage());
  Serial.printf("\n\n\n");
  delay(30);
}

void drawFont(const char name[], const GFXfont* f)
{
  //display.setRotation(0);
    display.fillScreen(GxEPD_WHITE);
//   display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
}
