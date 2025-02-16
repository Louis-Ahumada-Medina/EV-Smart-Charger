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
  //Listed Below

  //Code from SDWebServer
  /*
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the WebServer library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  */

  //Code from 


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
//#include "Simpletimer.h"

#include "../test_images/wifi.c"

//Webserver
#include <WiFi.h>
#include <NetworkClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

/************************************************************************
* Title: GPIO Assignments 
***********************************************************************/

//LCD
#define TFT_DC 2    
#define TFT_CS 0          
#define TFT_MOSI 3 //Shared With SD Card
#define TFT_CLK 6  //Shared With SD Card
#define TFT_RST 1  

//SD Card
#define SD_MISO 4   
#define SD_CS 5   

/************************************************************************
* Title: Varibles Assignments 
***********************************************************************/

//LCD
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

//Webserver
const char *host = "SmartChargerEV";
static bool hasSD = false;

/************************************************************************
* Title: Function Prototypes 
***********************************************************************/

//LCD_Functions
//void testFillScreen();
//void testText();

void GIFDraw(GIFDRAW *pDraw);

//SD_Card_Functions 
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void readFile(fs::FS &fs, const char *path);

//Timer






#endif



