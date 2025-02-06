/************************************************************************
* Title: SD_Card_Functions.cpp
* File Purpose: Stores SD card reelated functions
***********************************************************************/

#include "EV_Smart_Charger.h"

void sd_write_data(){

}

//Source: https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

//Source: https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  
  file.print(message);
  file.close();
}

//Source: https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}