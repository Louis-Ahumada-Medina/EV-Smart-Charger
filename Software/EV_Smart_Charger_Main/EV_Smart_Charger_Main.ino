//Getting Time: https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
/**********************************************************************************
* System Setup Declorations
**********************************************************************************/

//Dependancies
#include <WiFi.h>               //Webiste Functionality
#include <NetworkClient.h>      //Webiste Functionality
#include <WebServer.h>          //Webiste Functionality
#include <ESPmDNS.h>            //Webiste Functionality
#include "SPI.h"                //LCD Functionality
#include "SD.h"                 //SD Card Functionality
#include "FS.h"                 //SD Card Functionality
#include <Wire.h>               //PZEM004T(Power Sensor) Functionalitiy
#include "time.h"               //Time Gathering Functionality
#include "Adafruit_GFX.h"       //LCD Functionality
#include "Adafruit_ILI9341.h"   //LCD Functionality
#include <PZEM004Tv30.h>        //PZEM004T(Power Sensor) Functionalitiy

//SD Card  
#define SD_MISO 3  
#define SD_MOSI 46 
#define SD_CLK 8
#define SD_CS 9

//LCD Pin
#define TFT_DC 12    
#define TFT_CS 14         
#define TFT_MOSI 11 
#define TFT_CLK 10  
#define TFT_RST 13 

//PZEM1 Communication Pins
#define PZEM1_RX_PIN 21
#define PZEM1_TX_PIN 47
#define PZEM1_SERIAL Serial1

//PZEM2 Communication Pins
#define PZEM2_RX_PIN 45
#define PZEM2_TX_PIN 35
#define PZEM2_SERIAL Serial1

//RGB LED
#define LED_PIN 48

//Variables
const char *ssid = "Louis_Ahumada_iPhone";        //Internet variable
const char *password = "012345678";                //Internet variable
const char *host = "SmartChargerEV";              //Internet variable
const char* ntpServer = "pool.ntp.org";           //Location to get time from
const long  gmtOffset_sec = -28800;               //Local Time variable
const int   daylightOffset_sec = 3600;            //Local Time variable
static bool hasSD = 0;                            //System Status
static bool offlineMode = 1;                      //System Status 
String dataPacket = "000.0_00.0_00.0_0.00_0";     //Data
String relayState = "0";                          //Data
float voltage1 = 0;                         //Data
float current1 = 0;                          //Data
float wattage1 = 0;                          //Data
float voltage2 = 0;                         //Data
float current2 = 0;                          //Data
float wattage2 = 0;                          //Data
String voltage = "000.0";                         //Data
String current = "00.0";                          //Data
String wattage = "0.00";                          //Data
String temperature = "00.0";                      //Data
int timer = 0;                                    //Counts up to 5 minutes
int offset = 0;                                   //Timer offset
String startTime = "0";                           //Time stamp of system initialization
String endTime = "0";                             //5 minute Time stamp
String scheduleData = "0_0_0_0_0_0_0_0_0_0_0_0_0_0"; //Data

//Objects
WebServer server(80);                             //Wifi port
File uploadFile;                                  //MicroSD card
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST); //LCD
PZEM004Tv30 pzem1(PZEM1_SERIAL, PZEM1_RX_PIN, PZEM1_TX_PIN); //Power Sensor1
PZEM004Tv30 pzem2(PZEM2_SERIAL, PZEM2_RX_PIN, PZEM2_TX_PIN); //Power Sensor2

void setup() {
  Serial.begin(115200);

  //Set LED to Initializing Mode(White) color
  neopixelWrite(LED_PIN,64,64,64); 

  //Initialize LCD
  initLCD();

  //Sets up SDcard, wifi, and Server
  if (initSdCard()) { //Determine if SD card is operational
    //SD card is set up and operational

    //Sets up wifi
    if (initWifi()){ //Determine if Wifi is operational
      //Wifi is operational

      Serial.println("Entering Online Mode");
      offlineMode = false;

      delay(100);

      //Sets up time
      initTime();    

      //Sets up Scheduling feature
      //initScheduleData();

      //Initialize Server
      initServer();
    }
    else{ 
      //Enter into offline mode(no remote control)
      Serial.println("Entering Offline Mode");
      offlineMode = true;

      //Set LED to Offline Mode(BLUE)color
      neopixelWrite(LED_PIN,0,0,64); 
    }
  }
  else{
    //SD card is not operational
    Serial.println("Entering Offline Mode");
    offlineMode = true;

  }

  //Sets up data log file
  initPowerConsumptionFile();

  //Update
  requestDataUpdate();

  //Update LCD
  updateLCD();

  delay(1000); // Wait for setup to stabilize
}

void loop() {
  //Determine if in offlineMode
  if (!offlineMode){

    if (WiFi.status() == WL_CONNECTED){
      //Proceed to Remote control/Smart features 
      server.handleClient();
    }
    else{
      Serial.println("Error: Lost Wifi Connection");
      Serial.println("Entering Offline Mode");
      offlineMode = true;
    }
  }
  else{
    requestDataUpdate();
  }

  updateLCD();

  //Calculate how much time has passed
  timer = millis() - offset;

  //Enter if ~6 seconds have passed
  if (timer >= 360000 & !offlineMode){
    //Get current time
    offset = millis();
    endTime = getTime();
    
    //Store data to powerConsumption File
    addToPowerConsumptionLog();
  }
  else if(timer >= 360000){
    offset = millis();

    //Get current time
    endTime = String(endTime);
    //Store data to powerConsumption File
    addToPowerConsumptionLog();
  }

  delay(500);
}

/**********************************************************************************
* Functions for Initialization
**********************************************************************************/

//Returns true if SD/MicroSD card is operational
bool initSdCard(){
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS); //SD card pins

  if (SD.begin(SD_CS)) {
    Serial.println("SD Card initialized.");
    hasSD = true;
    return 1;
  }
  else{
    Serial.println("ERROR: SD Card Failed to Initialize");
    return 0;
  }
}

//Returns true if wifi is connected
bool initWifi(){
  //Wifi Set up
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  // Try to connect to Wifi connection
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 40) { 
    Serial.print(".");
    delay(1000);//wait 10 seconds
  }

  //Determine if connection was successful
  if (i == 41) {
    //Timeout
    Serial.println("\nCould not connect to " + String(ssid));
    return 0;
  }
  else{
    return 1;
  }
}

//Starts up Server
void initServer(){
  //Set up URL
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print("\nYou can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }

  //Set up server commands and operations
  server.on("/list", HTTP_GET, printDirectory);
  server.on("/edit", HTTP_DELETE, handleDelete);
  server.on("/edit", HTTP_PUT, handleCreate);
  server.on(
    "/edit", HTTP_POST,
    []() {
      returnOK();
    },
    handleFileUpload
  );
  server.onNotFound(handleNotFound);
  server.on("/powerControl", powerControl);  //Routine to handle powerControl GET
  server.on("/getData", sendData); //Routine to handle getData GET
  server.on("/sendTime", getTime); //Routine to handle getTime GET
  //server.on("/sendScheduleDatatoHost", receiveScheduleData);  //Routiune to handle sendScheduleDatatoHost
  //server.on("/getScheduleDatafromHost", sendScheduleData);  //Routiune to handle getScheduleDatafromHost


  //Begin Server
  server.begin();
  Serial.println("HTTP server started");
}

//Initiates what is needed for time keeping
void initTime(){

  //Set Up Time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  offset = millis();

  //Set Starting Point Time
  startTime = getTime();
  Serial.println("\nStart Time: "+startTime);
}

//Initializes PowerConsumptionLog file with starting time
void initPowerConsumptionFile(){
  String temp = "\nProgram Start Time," + startTime;
  const char * temp2 = temp.c_str();

  //Attempt to append to file
  if(!appendFile(SD,"/UserFiles/PowerConsumptionLog.txt",temp2)){
    //File does not exist to create and write to file
    writeFile(SD,"/UserFiles/PowerConsumptionLog.txt",temp2);
    Serial.println("Created File: PowerConsumptionLog.txt");
  }
  else{
    Serial.println("Appended File: PowerConsumptionLog.txt");
  }
}

//Initialize LCD
void initLCD(){
  //Initialize object values
  tft.begin();

  //Initalize screen orientation/position
  tft.setRotation(3);
  tft.setCursor(0, 0);

  //Turn LCD screen black
  tft.fillScreen(ILI9341_BLACK);
}

//Initializes Schedule data
void initScheduleData(){
  String storedSchedule = readFile(SD,"/UserFiles/Schedule.txt");

  if (storedSchedule != "N/A"| storedSchedule != scheduleData){
    scheduleData = storedSchedule;
  }
  else{
    const char * temp = scheduleData.c_str();
    scheduleData = temp;

    //Initialize schedule file
    writeFile(SD,"/UserFiles/Schedule.txt",temp);
  }
  
  Serial.println("\nDefault Schedule: " + scheduleData);
}

/*********************************************************************************
* Functions for Loading Website From SD Card
**********************************************************************************/
void returnOK() {
  server.send(200, "text/plain", "");
}

void returnFail(String msg) {
  server.send(500, "text/plain", msg + "\r\n");
}

bool loadFromSdCard(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (server.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

void handleFileUpload() {
  if (server.uri() != "/edit") {
    return;
  }
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (SD.exists((char *)upload.filename.c_str())) {
      SD.remove((char *)upload.filename.c_str());
    }
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
    Serial.print("Upload: START, filename: ");
    Serial.println(upload.filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    Serial.print("Upload: WRITE, Bytes: ");
    Serial.println(upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    Serial.print("Upload: END, Size: ");
    Serial.println(upload.totalSize);
  }
}

void deleteRecursive(String path) {
  File file = SD.open((char *)path.c_str());
  if (!file.isDirectory()) {
    file.close();
    SD.remove((char *)path.c_str());
    return;
  }

  file.rewindDirectory();
  while (true) {
    File entry = file.openNextFile();
    if (!entry) {
      break;
    }
    String entryPath = path + "/" + entry.name();
    if (entry.isDirectory()) {
      entry.close();
      deleteRecursive(entryPath);
    } else {
      entry.close();
      SD.remove((char *)entryPath.c_str());
    }
    yield();
  }

  SD.rmdir((char *)path.c_str());
  file.close();
}

void handleDelete() {
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (path == "/" || !SD.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }
  deleteRecursive(path);
  returnOK();
}

void handleCreate() {
  if (server.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if (path == "/" || SD.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }

  if (path.indexOf('.') > 0) {
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    if (file) {
      file.write(0);
      file.close();
    }
  } else {
    SD.mkdir((char *)path.c_str());
  }
  returnOK();
}

void printDirectory() {
  if (!server.hasArg("dir")) {
    return returnFail("BAD ARGS");
  }
  String path = server.arg("dir");
  if (path != "/" && !SD.exists((char *)path.c_str())) {
    return returnFail("BAD PATH");
  }
  File dir = SD.open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/json", "");

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }

    String output;
    if (cnt > 0) {
      output = ',';
    }

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.path();
    output += "\"";
    output += "}";
    server.sendContent(output);
    entry.close();
  }
  server.sendContent("]");
  dir.close();
}

void handleNotFound() {
  if (hasSD && loadFromSdCard(server.uri())) {
    return;
  }
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print(message);
}

/**********************************************************************************
* Functions Interacting with Internet
**********************************************************************************/

//Receive Power on/off command
void powerControl() {
  String t_state = server.arg("powerState");

  Serial.println("Recived from Server: "+t_state);

  if(t_state == "0")  {
    relayState = "0"; //Feedback parameter
    neopixelWrite(LED_PIN,64,0,0); //Set LED to OFF (Red)
  }
  else  {
    relayState = "1"; //Feedback parameter 
      neopixelWrite(LED_PIN,0,64,0); //Set LED to ON (Green)
  }
  server.send(200, "text/plane", relayState); //Send web page
}

//Send data to server
void sendData(){
  requestDataUpdate();
  //send Data(V,A,T,P,R)
  Serial.println("Sending data values to Server: "+dataPacket);
  server.send(200, "text/plane", dataPacket);
}

//Get local time form NTPServer
String getTime(){
  //Time objec and variables
  struct tm timeinfo;
  char temp[10];
  String result = "";

  //Get Time from Server
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "0";
  }

  //Get Weekday
  strftime(temp,10, "%A", &timeinfo);
  result = result + String(temp) + ",";

  //Get Month
  strftime(temp,10, "%B", &timeinfo);
  result = result + String(temp) + ",";

  //Get Day of Month
  strftime(temp,10, "%d", &timeinfo);
  result = result + String(temp) + ",";

  //Get Year
  strftime(temp,10, "%Y", &timeinfo);
  result = result + String(temp) + ",";

  //Get Hour
  strftime(temp,10, "%H", &timeinfo);
  result = result + String(temp) + ":";

  //Get Minute
  strftime(temp,10, "%M", &timeinfo);
  result = result + String(temp) + ":";

  //Get Second
  strftime(temp,10, "%S", &timeinfo);
  result = result + String(temp) + "\n";

  return result;
}

//Updates Sensor Values 
void requestDataUpdate(){

  //Call functions to assign value to stuff
  voltage1 = pzem1.voltage();
  current1 = pzem1.current();
  wattage1 = pzem1.power();

  voltage2 = pzem2.voltage();
  current2 = pzem2.current();
  wattage2 = pzem2.power();


  voltage = String(round((((voltage1 + voltage2))*10))/10);
  current = String(round(((((current1 + current2)/2))*10))/10);
  wattage = String(wattage1 + wattage2);

  //Format: dataPacket = "Voltage_Current_Temp_Power_RelayState"
  dataPacket = voltage + "_" + current + "_" + temperature + "_" + wattage + "_" + relayState;
}

//Recieve Schedule Data from Server
void receiveScheduleData() {
  scheduleData = server.arg("schedule");
  Serial.println("Recived data: "+scheduleData);

  const char * temp = scheduleData.c_str();

  //Update schedule file
  writeFile(SD,"/UserFiles/Schedule.txt",temp);
  Serial.println("Updated Schedule file");

  server.send(200, "text/plane", scheduleData); //Send scheduleData value to web page
}

//Send Schedule Data to Server
void sendScheduleData() {
  //Retrive schedule data from file
  scheduleData = readFile(SD, "/UserFiles/Schedule.txt");

  //Send schedule data to sever
  Serial.println("Sending schedule to Server: "+scheduleData);
  server.send(200, "text/plane", scheduleData);
}

/**********************************************************************************
* Functions for SD card
**********************************************************************************/

//From SD Card Library Example
String readFile(fs::FS &fs, const char *path) {
  String temp = "";

  //Create File Object
  File file = fs.open(path);

  //Check if file exist
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "N/A";
  }

  //Read from file
  while (file.available()) {
    Serial.write(file.read());
  }

  temp = file.readString();

  file.close();

  return temp;
}

//From SD Card Library Example
void writeFile(fs::FS &fs, const char *path, const char *message) {
  //Create File Object
  File file = fs.open(path, FILE_WRITE);

  //Check if File Exists
  if (!file) {
    return;
  }

  //Write to File
  if (file.print(message)) {
    //Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

//From SD Card Library Example
bool appendFile(fs::FS &fs, const char *path, const char *message) {
  //Varibles and File Object
  bool sucess = false;
  File file = fs.open(path, FILE_APPEND);

  //Check to See if File Exists
  if (!file) {
    Serial.println("Failed to open file for appending");
  }

  //Append File
  if (file.print(message)) {
    //Serial.println("File appended");
    sucess = true;
  } 
  else {
    Serial.println("Append failed");
    sucess = false;
  }
  file.close();
  return sucess;
}

//Saves Sensor Data, and End Time to Text File
void addToPowerConsumptionLog(){
  //Format Values
  String temp = "\nTime," + endTime + ",Voltage," + voltage + ",Current," + current + ",Temperature," + temperature + ",Wattage," + wattage + "\n";

  //Create required variable type for values
  const char * temp2 = temp.c_str();

  //Append time and sensor values to file
  appendFile(SD,"/UserFiles/PowerConsumptionLog.txt",temp2);
}

/**********************************************************************************
* Functions for LCD
**********************************************************************************/

//Updates what is displayed on the LCD
//Size 3: ?, ~25(gap 4)
//size 2: ~11(gap 1), ~18(gap 4)
void updateLCD(){
  //Set title size
  tft.fillRect(0, 0, 320, 22, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setCursor(0,0);
  tft.setTextSize(3);

  //Determine if program is in Online or offline mode
  if (offlineMode){
    tft.println("Offline Mode");

    //Set regular text size
    tft.setTextSize(2);

    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(0,30);
    tft.print("SD Card: ");

    tft.fillRect(0, 200, 320, 16, ILI9341_BLACK);

    //Determines if memory card is operational state
    if (hasSD){
      //memory card is operational state
      tft.fillRect(99, 30, 221, 16, ILI9341_BLACK);
      tft.setTextColor(ILI9341_GREEN);
      tft.setCursor(99,30);
      tft.print("Connected\n");

      //Steps and troubleshooting wifi
      //tft.println("If you want to enter Online mode ensure:");
      //tft.println("-Wifi is within range");
      //tft.println("-Wifi credentials are correct(MicroSD/SD card -> config.txt)");
      //tft.println("-Wifi Network is 2.4GHz");
      //tft.println("Disconnect and reconnect EV Smart Charger from power to switch modes");
    }
    else{
      //Memory device status
      tft.fillRect(99, 30, 221, 16, ILI9341_BLACK);
      tft.setCursor(99,30);
      tft.setTextColor(ILI9341_RED);
      tft.print("Disconnected");

      //Steps and troubleshooting memory device
      //tft.println("");
      //tft.println("If you want to enter Online mode ensure:");
      //tft.println("-MicroSD or SD card is inserted");
      //tft.println("Disconnect and reconnect EV Smart Charger from power to switch modes");
    }
    //Could not connect to internet state
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(0,50);
    tft.print("Wifi: ");

    tft.setTextColor(ILI9341_RED);
    tft.fillRect(66, 50, 254, 16, ILI9341_BLACK);
    tft.setCursor(66,50);
    tft.print("Disconnected");

  }
  else{
    tft.println("Online Mode");

    //Set regular text size
    tft.setTextSize(2);

    //Display memory device and wifi status
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(0,30);
    tft.println("SD Card: ");

    tft.setTextColor(ILI9341_GREEN);
    tft.fillRect(99, 30, 221, 16, ILI9341_BLACK);
    tft.setCursor(99,30);
    tft.print("Connected\n");

    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(0,50);
    tft.println("Wifi: ");

    tft.setTextColor(ILI9341_GREEN);
    tft.fillRect(66, 50, 254, 16, ILI9341_BLACK);
    tft.setCursor(66,50);
    tft.print("Connected\n");

    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(0,200); 
    tft.print("URL: ");
    tft.setCursor(55,200); 
    tft.print(host);
    tft.print(".local");
  }

  //Print data values
  tft.setTextColor(ILI9341_WHITE); 
  tft.setCursor(0,93); 
  tft.print("Voltage(Volts): ");
  tft.fillRect(187, 93, 133, 16, ILI9341_BLACK);
  tft.setCursor(187,93); 
  tft.print(voltage);

  tft.setCursor(0,111); 
  tft.print("Current(Amps): ");
  tft.fillRect(176, 111, 144, 15, ILI9341_BLACK);
  tft.setCursor(176,111); 
  tft.print(current);

  tft.setCursor(0,129); 
  tft.print("Temperature(Celcius): "); 
  tft.fillRect(253, 129, 67, 15, ILI9341_BLACK);
  tft.setCursor(253,129);
  tft.print(temperature);

  tft.setCursor(0,147); 
  tft.print("Power(Wattage): ");
  tft.fillRect(187, 147, 133, 15, ILI9341_BLACK);
  tft.setCursor(187,147); 
  tft.print(wattage);
}

/**********************************************************************************
* Functions for Temp. Sensor?
**********************************************************************************/

