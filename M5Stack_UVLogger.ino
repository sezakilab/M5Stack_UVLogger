#include <M5Stack.h>
#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include "DHT12.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_VEML6075.h"
#include <Adafruit_BMP280.h>
#include "String.h"
#include "TinyGPS++.h"
      
// GPS
HardwareSerial GPSRaw(2);
TinyGPSPlus gps;

// UV sensor
Adafruit_VEML6075 uv = Adafruit_VEML6075();

// WiFi
WiFiClient client;
File file;

// NTP
const char* ntpServer = "ntp.nict.jp";
const long  gmtOffset_sec = 3600 * 9;
const int   daylightOffset_sec = 0;
struct tm timeinfo;

// ENV Unit
DHT12 dht12; 
Adafruit_BMP280 bme;

// SD card
File data_file;
File gps_file;
const uint8_t cs_SD = 5;
const char* data_fname = "/data.csv";  
const char* gps_fname = "/gps.csv";

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Power.begin();
  
  // UV sensor
  Wire.begin();
  Serial.println("VEML6075 Full Test");
  if (! uv.begin()) {
    Serial.println("Failed to communicate with VEML6075 sensor, check wiring?");
  }
  Serial.println("Found VEML6075 sensor");
  uv.setIntegrationTime(VEML6075_100MS);

  // ENV sensor
  while (!bme.begin(0x76)){  
    M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
  }
  M5.Lcd.clear(BLACK);
    
  // Connect WiFi
  String ssid     = getSSIDFromSD("/WIFI.txt");
  String password = getPasswordFromSD("/WIFI.txt");
  WiFi.begin(ssid.c_str(), password.c_str());  //  Wi-Fi APに接続
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
      delay(1000);
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.println("Could not connect WiFi");
  }

  // NTP Time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Disconnect WiFi
//  WiFi.disconnect(true);
//  WiFi.mode(WIFI_OFF);

//  GPSRaw.begin(9600);

  // SD card
  SD.begin(cs_SD, SPI, 24000000, "/sd");

  if (!SD.exists(data_fname)) {
    data_file = SD.open(data_fname, FILE_APPEND);
    if(data_file.println("device_time,datetime,UV-Index,UVA,UVB,Temp,Hum,Pressure")){
      M5.Lcd.printf("[Data->SD] Success\n");
    }
    data_file.close();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);

  // elapse time
  M5.Lcd.printf("%lu\n",millis());

  // UV Sensor
  M5.Lcd.printf("UVA: %.2f, UVB: %.2f\n", uv.readUVA(), uv.readUVB());
  M5.Lcd.printf("UV Index: %.2f\n", uv.readUVI());

  // ENV
  float tmp = dht12.readTemperature();
  float hum = dht12.readHumidity();
  float pressure = bme.readPressure();
  // Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%  Pressure: %0.2fPa\r\n", tmp, hum, pressure);
  M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPressure:%2.0fPa\r\n", tmp, hum, pressure);

  /// NTP
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("Failed to obtain time");
    return;
  }
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("%04d-%02d-%02d %02d:%02d:%02d\n" 
                ,timeinfo.tm_year + 1900
                ,timeinfo.tm_mon + 1
                ,timeinfo.tm_mday
                ,timeinfo.tm_hour
                ,timeinfo.tm_min
                ,timeinfo.tm_sec
                );
  

  M5.Lcd.printf("\n");
  // eplase_time, datetime, UV-Index, UVA, UVB, temp,hum,pressure
  data_file = SD.open(data_fname, FILE_APPEND); // FILE_WRITE);
  if(data_file.printf("%lu,%04d-%02d-%02d %02d:%02d:%02d,%.2f,%.2f,%.2f,%2.2f,%0.2f,%0.2f\n"
                ,millis()
                ,timeinfo.tm_year + 1900
                ,timeinfo.tm_mon + 1
                ,timeinfo.tm_mday
                ,timeinfo.tm_hour
                ,timeinfo.tm_min
                ,timeinfo.tm_sec
                ,uv.readUVI()
                ,uv.readUVA()
                ,uv.readUVB()
                ,tmp
                ,hum
                ,pressure)){
    M5.Lcd.printf("[Data->SD] Success\n");
  }else{
    M5.Lcd.printf("[Data->SD] Failure\n");
  }
  data_file.close();

  delay(1000);

  // GPS
//  while(GPSRaw.available()) {
//    char c = GPSRaw.read();
//    Serial.print(c);
//    gps_file = SD.open(gps_fname, FILE_APPEND);
//    if(!gps_file.print(c)){
//      M5.Lcd.println("[GPS Data->SD] Failure\n");
//    }
//    gps_file.close();
//    
//    if (gps.encode(c)) {
//       if (gps.location.isUpdated()) {
//        M5.Lcd.print("LAT: "); M5.Lcd.println(gps.location.lat(),6);
//        M5.Lcd.print("LNG: "); M5.Lcd.println(gps.location.lng(),6); 
//        M5.Lcd.print("ALT: "); M5.Lcd.println(gps.altitude.meters());
//        TinyGPSDate d = gps.date;
//        TinyGPSTime t = gps.time;
//        if (d.isValid()) {
//          char sz[32];
//          sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
//          M5.Lcd.println(sz);
//        }
//        if (t.isValid()){
//          char sz[32];
//          sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
//          M5.Lcd.println(sz);
//        }
//       }
//      break;
//    }
//  }

  // smartDelay(900);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPSRaw.available()){
      gps.encode(GPSRaw.read());      
    }
  } while (millis() - start < ms);
}

/**
 * 一行目の文字列を読み込む
 */
String getSSIDFromSD(const char *file_name) {
  String str;
  file = SD.open(file_name, FILE_READ);
  if(file){
      while (file.available()) {
        char c = char(file.read());
        if(c == '\n'){
          break;          
        }
        str += c;
      }
  } else{
      Serial.println(" error...");
  }
  file.close();
  return str;
}

/**
 * 二行目の文字列を読み込む
 * TODO: getSSIDFromSD(char*)とマージする
 */
String getPasswordFromSD(const char *file_name) {
  String str;
  file = SD.open(file_name, FILE_READ);
  if(file){
    bool flag = false;
    while (file.available()) {
      char c = char(file.read());
      if(c == '\n'){
        flag = true;       
        continue;
      }
      if(flag){
        str += c;        
      }
    }
  } else{
      Serial.println(" error...");
  }
  file.close();
  return str;
}
