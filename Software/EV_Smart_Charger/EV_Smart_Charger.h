/************************************************************************
* Title: EV_Smart_Charger.h
* File Purpose:
* - License
* - Library Definitions
* - GPIO Assigments
* - Variable Assigments
* - Function Prototypes
***********************************************************************/

/************************************************************************
* Title: License
***********************************************************************/

  /*
  *  MIT License
  *
  *  Copyright (c) 2025 Louis Ahumada Medina
  *
  *  Permission is hereby granted, free of charge, to any person obtaining a copy
  *  of this software and associated documentation files (the "Software"), to deal
  *  in the Software without restriction, including without limitation the rights
  *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  *  copies of the Software, and to permit persons to whom the Software is
  *  furnished to do so, subject to the following conditions:
  *
  *  The above copyright notice and this permission notice shall be included in all
  *  copies or substantial portions of the Software.
  *
  *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  *  SOFTWARE.
  */

  //Note this code is the culmination of various sources with thier own license

/************************************************************************
* Title: Libraries & Dependacies
***********************************************************************/

//No Redefinitions
#ifndef EV_SMART_CHARGER_H
#define EV_SMART_CHARGER_H

//For SPI Functionality
#include "SPI.h"

//LCD
#include "Adafruit_GFX.h" 
#include "Adafruit_ILI9341.h"
#include "AnimatedGIF.h"

//SD Card
#include "FS.h"
#include "SD.h"

//Timer
#include "Simpletimer.h"

#include "../test_images/Demoimage.h"

/************************************************************************
* Title: GPIO Assignments 
***********************************************************************/

//LCD
#define TFT_DC 8    
#define TFT_CS 7          
#define TFT_MOSI 10 //Shared With SD Card
#define TFT_CLK 19  //Shared With SD Card
#define TFT_RST 11  

//SD Card
#define SD_MISO 15   
#define SD_CS 0  

/************************************************************************
* Title: Varibles Assignments 
***********************************************************************/

//LCD
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

/************************************************************************
* Title: Function Prototypes 
***********************************************************************/

//LCD_Functions
void testFillScreen(Adafruit_ILI9341 &tft);
void testText(Adafruit_ILI9341 &tft);
void GIFDraw(GIFDRAW *pDraw);

//SD_Card_Functions 
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void readFile(fs::FS &fs, const char *path);

//Timer



#endif



