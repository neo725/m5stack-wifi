#include <Arduino.h>
#include <M5Stack.h>
#include <String.h>
#include <SPI.h>
#include <SD.h>

#include "WiFi.h"
#include "time.h"

#include "utility.h"
#include "IO.h"
#include "StringSplitter.h"
#include "Colors.h"

#define MAX_ARRAY_SIZE 10

String ssid = "";
String password = "";

// wifi.txt => wifi information, UTF-8
const char *wifi_setting_file_path = "/wifi.txt";

const char *ntpServer = "tick.stdtime.gov.tw";
const long  gmtOffset_sec = 60 * 60 * 8; // GMT+8
const int   daylightOffset_sec = 3600;

const int16_t paddingX = 15;
const int16_t topY = 15;
const int16_t spaceY = 5;
int16_t lineHeight = 20;

int status = WL_IDLE_STATUS;     // the Wifi radio's status
bool isSDready = false;

/* test SD */
String cal;
String lum;
String unit;

bool brght = 0;
bool ftmt = 1;

int x = 1680;

int16_t getYPosition(int8_t line) {
  line = line - 1;
  int16_t y = topY;

  if (line == 0) return y;

  // more then one line
  y += (spaceY * line) + (lineHeight * line);

  return y;
}



void ReadWiFiDataFromTF() {
  isSDready = false;

  if (SD.exists(wifi_setting_file_path) == false) {
    M5.Lcd.setCursor(paddingX, getYPosition(1));
    M5.Lcd.println("wifi setting file not exists");
    return;
  }

  // File f = SD.open(wifi_setting_file_path, FILE_READ);
  // readFile(f);

  File file = SD.open(wifi_setting_file_path, FILE_READ);
  if (file) {
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(paddingX, getYPosition(1));
    //M5.Lcd.print(f.read());
    while (file.available()) {
      // byte b = file.read();
      // Serial.println(char(b));
      Serial.print(file.readString());
    }
    file.close();

    isSDready = true;
  }
  else {
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(paddingX, getYPosition(1));
    M5.Lcd.println("open error wifi.txt");
  }

}

void PrintWeekday(tm timeinfo) {
  M5.Lcd.setCursor(paddingX, getYPosition(1));
  M5.Lcd.print(&timeinfo, "%A");
}

void PrintDateTime(tm timeinfo) {
  // Date
  M5.Lcd.setCursor(paddingX, getYPosition(2));
  M5.Lcd.print(&timeinfo, "%B %d %Y");
  // Time
  // M5.Lcd.setCursor(paddingX, getYPosition(3));
  // M5.Lcd.print("_________");

  // drawRect(x, y, w, h, color);
  // M5.Lcd.drawRect(paddingX, getYPosition(3), 
  //   200, spaceY + lineHeight, 
  //   TFT_BLACK);

  M5.Lcd.setCursor(paddingX, getYPosition(3));
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.print(" "); // lcd.print(" ");

  M5.Lcd.setCursor(paddingX, getYPosition(3));
  M5.Lcd.print(&timeinfo, "%H:%M:%S");

  M5.update();
}

void refreshDateTimeOnly() {
  if (status != WL_CONNECTED) return;

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.println("Failed to obtain time");
    return;
  }

  PrintDateTime(timeinfo);
}

void printLocalTime() {
  if (status != WL_CONNECTED) return;

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.println("Failed to obtain time");
    return;
  }
  // //M5.Lcd.setCursor(paddingX, 10);
  // M5.Lcd.setCursor(paddingX, getYPosition(1));
  // // M5.Lcd.println(&timeinfo, "%A, \r\n%B %d %Y %H:%M:%S");
  // M5.Lcd.print(&timeinfo, "%A");
  PrintWeekday(timeinfo);
  // //M5.Lcd.setCursor(paddingX, 38);
  // M5.Lcd.setCursor(paddingX, getYPosition(2));
  // M5.Lcd.print(&timeinfo, "%B %d %Y %H:%M:%S");
  PrintDateTime(timeinfo);
}

void setup() {
  // Serial.begin(9600);

  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }

  // Serial.println("Serial is ready");

  // Serial.println("Program start running...");

  /* ====== M5 ====== */
  M5.begin();

  Wire.begin();

  Serial.begin(115200);

  // Lcd display
  M5.Lcd.setBrightness(100);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  // Page Header
  // M5.Lcd.fillScreen(BLACK);
  // M5.Lcd.setCursor(0, 05);
  // M5.Lcd.printf("Testing SD Card Functions:\r\n");

  // Line test
  for (int i = 0; i < 0; i++) {
    //char * level = (char *)(i + 1);
    String level = String(i);

    // M5.Lcd.fillScreen(BLACK);
    M5.Lcd.clearDisplay();

    M5.Lcd.setCursor(paddingX, getYPosition(1));
    M5.Lcd.println("This is level " + level);

    M5.Lcd.setCursor(paddingX, getYPosition(2));
    M5.Lcd.println("This is Line # 2");

    M5.Lcd.setCursor(paddingX, getYPosition(3));
    M5.Lcd.println("This is Line # 3");

    delay(3000);

    lineHeight -= 1;
  }
  
  // return;

  /////////////////////////////////////////////////////////////////////////
  // cal = String("cal = ") + String(x) + "\r\n";
  // lum = String("lum = ") + String(brght) + "\r\n";
  // unit = String("unit = ") + String(ftmt) + "\r\n";

  // writeFile(SD, "/cal.txt", cal.c_str()); // questa funziona!!!!
  // writeFile(SD, "/lum.txt", lum.c_str()); // questa funziona!!!!
  // writeFile(SD, "/unit.txt", unit.c_str()); // questa funziona!!!!
  /////////////////////////////////////////////////////////////////////////

  M5.Lcd.print("");
  // Print blank line on screen
  M5.Lcd.printf(" \n  ");
  // Print blank line on screen
  M5.Lcd.printf(" \n  ");

  String result = "";
  int length = 0;

  readFile(SD, wifi_setting_file_path, result, length);

  result.replace("\r", "");
  
  M5.Lcd.println();

  StringSplitter *splitter = 
    new StringSplitter(result, '\n', MAX_ARRAY_SIZE);

  int itemCount = splitter->getItemCount();
  // M5.Lcd.print("itemCount : ");
  // M5.Lcd.println(itemCount);

  // M5.Lcd.println();

  int lineIndex = 0;

  for(int i = 0; i < itemCount; i++) {
    String item = splitter->getItemAtIndex(i);
    item.trim();

    if (item.startsWith("#")) continue;

    if (lineIndex == 0) {
      ssid = item;
    }
    else if (lineIndex == 1) {
      password = item;
    }

    lineIndex += 1;
  }

  WiFi.disconnect();

  // ReadWiFiDataFromTF();
  
  // if (WiFi.status() == WL_NO_SHIELD) {
  //   Serial.println("WiFi shield not present !!");
  //   // don't continue:
  //   while (true);
  // }

  // temp to make always return
  // if (isSDready == false) return;
  // return;

  //connect to WiFi
  M5.Lcd.printf("Connecting to %s ", ssid.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      M5.Lcd.print('.');
  }

  status = WiFi.status();

  M5.Lcd.clearDisplay();

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  if (status == WL_CONNECTED) {
    // M5.Lcd.clearDisplay();
    //M5.Lcd.fillScreen(BLACK);

    // M5.Lcd.setCursor(0, 0);
    printLocalTime();
    // refreshDateTimeOnly();
  }

  M5.update();

  delay(1000);
}