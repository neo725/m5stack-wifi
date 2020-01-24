#include <Arduino.h>
#include <M5Stack.h>
#include <String.h>
#include "WiFi.h"
#include "time.h"


char ssid[]       = "NeoHome2130";
char password[]   = "29550674";


const char* ntpServer = "tick.stdtime.gov.tw";
const long  gmtOffset_sec = 60 * 60 * 8; // GMT+8
const int   daylightOffset_sec = 3600;

const int16_t topY = 6;
const int16_t baseY = 10;
const int16_t lineHeight = 28;

int status = WL_IDLE_STATUS;     // the Wifi radio's status

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  // byte bssid[6];
  
  // uint8_t bssid[6];
  // WiFi.BSSID(bssid);
  // Serial.print("BSSID: ");
  // Serial.print(bssid[5], HEX);
  // Serial.print(":");
  // Serial.print(bssid[4], HEX);
  // Serial.print(":");
  // Serial.print(bssid[3], HEX);
  // Serial.print(":");
  // Serial.print(bssid[2], HEX);
  // Serial.print(":");
  // Serial.print(bssid[1], HEX);
  // Serial.print(":");
  // Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  // byte encryption = WiFi.encryptionType();
  // Serial.print("Encryption Type:");
  // Serial.println(encryption, HEX);
  // Serial.println();
}

int16_t getYPosition(int8_t line) {
  line = line - 1;

  if (line == 0) return baseY + topY;

  int16_t y = 
    (lineHeight * line) + (baseY * line);

  return y;
}

void PrintWeekday(tm timeinfo) {
  M5.Lcd.setCursor(15, getYPosition(1));
  M5.Lcd.print(&timeinfo, "%A");
}

void PrintDateTime(tm timeinfo) {
  M5.Lcd.setCursor(15, getYPosition(2));
  M5.Lcd.print(&timeinfo, "%B %d %Y %H:%M:%S");
}

void refreshDateTimeOnly() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.println("Failed to obtain time");
    return;
  }

  PrintDateTime(timeinfo);
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.println("Failed to obtain time");
    return;
  }
  // //M5.Lcd.setCursor(15, 10);
  // M5.Lcd.setCursor(15, getYPosition(1));
  // // M5.Lcd.println(&timeinfo, "%A, \r\n%B %d %Y %H:%M:%S");
  // M5.Lcd.print(&timeinfo, "%A");
  PrintWeekday(timeinfo);
  // //M5.Lcd.setCursor(15, 38);
  // M5.Lcd.setCursor(15, getYPosition(2));
  // M5.Lcd.print(&timeinfo, "%B %d %Y %H:%M:%S");
  PrintDateTime(timeinfo);
}


void setup() {
  M5.begin();
  WiFi.disconnect();

  M5.Lcd.setTextSize(2);
  
  // if (WiFi.status() == WL_NO_SHIELD) {
  //   Serial.println("WiFi shield not present !!");
  //   // don't continue:
  //   while (true);
  // }

  //connect to WiFi
  M5.Lcd.printf("Connecting to %s ", ssid);

  WiFi.begin(ssid, password);
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
  // M5.Lcd.clearDisplay();
  M5.Lcd.setCursor(0, 0);
  // printLocalTime();
  refreshDateTimeOnly();

  delay(1000);
}