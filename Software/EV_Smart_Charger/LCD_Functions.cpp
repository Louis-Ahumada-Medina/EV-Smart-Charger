/************************************************************************
* Title: LCD_Functions.cpp
* File Purpose: Stores LCD Controlling Functions
***********************************************************************/

/************************************************************************
* Title: Citations
***********************************************************************/

  /*
  * Source:adafruit graphic test Example
  * Date:
  * Use:
  */

#include "EV_Smart_Charger.h"

void testFillScreen(Adafruit_ILI9341 &tft) {
  tft.fillScreen(ILI9341_RED);
}

void testText(Adafruit_ILI9341 &tft) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
}
