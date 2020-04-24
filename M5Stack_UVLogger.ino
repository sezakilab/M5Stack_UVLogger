/*
    note: need add library Adafruit_BMP280 from library manage
*/

#include <M5Stack.h>
#include <Wire.h> //The DHT12 uses I2C comunication.
#include "Adafruit_VEML6075.h"

Adafruit_VEML6075 uv = Adafruit_VEML6075();
#define SCLPIN 22                     // I2C SCL GPIO
#define SDAPIN 21                     // I2C SDA GPIO

// File f

// const uint8_t cs_SD = 5;
// const char* fname = "/data.csv";

void setup() {
    M5.begin();
    M5.Power.begin();
    Wire.begin();
    
    M5.Lcd.setTextSize(2);

    Serial.println("VEML6075 Full Test");
    if (! uv.begin()) {
      Serial.println("Failed to communicate with VEML6075 sensor, check wiring?");
    }
    Serial.println("Found VEML6075 sensor");
    uv.setIntegrationTime(VEML6075_100MS);
    // Get the integration constant and print it!
    Serial.print("Integration time set to ");
    switch (uv.getIntegrationTime()) {
      case VEML6075_50MS: Serial.print("50"); break;
      case VEML6075_100MS: Serial.print("100"); break;
      case VEML6075_200MS: Serial.print("200"); break;
      case VEML6075_400MS: Serial.print("400"); break;
      case VEML6075_800MS: Serial.print("800"); break;
    }
    Serial.println("ms");
    // uv.begin();
    // uv.begin(); 
    // uv.setIntegrationTime(VEML6075_100MS);
    
//    Serial.println(F("ENV Unit(DHT12 and BMP280) test..."));

//    while (!bme.begin(0x76)){  
//      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
//      M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
//    }
//    M5.Lcd.clear(BLACK);
//    M5.Lcd.println("ENV Unit test...");

//    Serial.begin(115200);
    // SD.begin(cs_SD, SPI, 24000000, "/sd");
}

void loop() {
 
//    float tmp = dht12.readTemperature();
//    float hum = dht12.readHumidity();
//    float pressure = bme.readPressure();
//    Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%  Pressure: %0.2fPa\r\n", tmp, hum, pressure);
//
//    M5.Lcd.setCursor(0, 0);
//    M5.Lcd.setTextColor(WHITE, BLACK);
//    M5.Lcd.setTextSize(3);
//    M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPressure:%2.0fPa\r\n", tmp, hum, pressure);
//
//    f = SD.open(fname, FILE_APPEND); // FILE_WRITE);
//    if(f.println(String(tmp) + "," +String(hum) + "," + String(pressure))){
//      M5.Lcd.printf("Good!\r\n");
//    }else{
//      M5.Lcd.printf("Failed\r\n");
//    }
//    f.close();

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(40, 40);
    M5.Lcd.printf("UVA: %.2f, UVB: %.2f", uv.readUVA(), uv.readUVB());
    M5.Lcd.setCursor(40, 80);
    M5.Lcd.printf("UV Index: %.2f", uv.readUVI());
    
    delay(1000);
}
