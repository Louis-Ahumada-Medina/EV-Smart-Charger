/************************************************************************
* Title: EV_Smart_Charger.ino
* File Purpose: Main Implementation File
* Info: Vitsit Git Hub Repository for more information about this 
* project
* Git Hub Repository Code: 
* https://github.com/Louis-Ahumada-Medina/EV-Smart-Charger?tab=MIT-1-ov-file
***********************************************************************/

#include "EV_Smart_Charger.h"
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
AnimatedGIF gif;

void setup() {
  //Start Serial
  Serial.begin(9600);
  Serial.println("Program Initiating...");

  //Set up LCD
  tft.begin();  //LCD
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.fillScreen(ILI9341_BLUE);
  gif.begin(LITTLE_ENDIAN_PIXELS);

  //Play Gif on LCD
  //if (gif.open((uint8_t *)Demoimage, sizeof(Demoimage), GIFDraw))
  //{
  //  while (gif.playFrame(true, NULL))
  //  {      
  //  }
  //  gif.close();
  //}

  SPI.begin(TFT_CLK, SD_MISO, TFT_MOSI, SD_CS); //SD Card
  //Check if SD card inserted
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    //////////////////////////////////////////////////Initiate no logging mode
  }

  writeFile(SD, "/DataLog.txt", "Current: 5A");

  Serial.println("~~Initiation Finished~~");
}

void loop(void) {

}




//From exmaple
// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw) {
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > DISPLAY_WIDTH)
       iWidth = DISPLAY_WIDTH - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
       return; 
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < iWidth)
      {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
             *d++ = usPalette[c];
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          tft.startWrite();
          tft.setAddrWindow(pDraw->iX+x, y, iCount, 1);
          tft.writePixels(usTemp, iCount, false, false);
          tft.endWrite();
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
             iCount++;
          else
             s--; 
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<iWidth; x++)
        usTemp[x] = usPalette[*s++];
      tft.startWrite();
      tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
      tft.writePixels(usTemp, iWidth, false, false);
      tft.endWrite();
    }
}