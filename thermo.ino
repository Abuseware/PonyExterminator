#include <OneWire.h>
#include <DallasTemperature.h>

#include <LiquidCrystal.h>

#include <SPI.h>
#include <SD.h>

#define ONEWIRE_PIN 8

#define SD_CS_PIN 10

#define LCD_RS 7
#define LCD_E 6
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#define SENSOR_RES 12

#define CHECK_INTERVAL 5

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorAddress[10];
uint8_t sensorCount = 0;
float sensorRead[10];
String fileName = "temp.csv";

LiquidCrystal LCD(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  //while(!Serial);
  //Serial.begin(9600);

  LCD.begin(16, 2);

  LCD.print(F("Karta: "));

  // see if the card is present and can be initialized:
  if(!SD.begin(SD_CS_PIN)) {
    LCD.print(F("BLAD!"));

    // don't do anything more:
    while(1);
  }

  LCD.print(F("OK"));

  sensors.begin();
  sensorCount = sensors.getDeviceCount();

  LCD.setCursor(0, 1);
  LCD.print(F("Sensory: "));
  LCD.print(sensorCount, DEC);

  delay(1000);

  for(uint16_t i = 0; i < 65535; i++) {
    fileName = "temp_" + String(i, DEC) + ".csv";
    if(!SD.exists(fileName)) {
      LCD.clear();
      LCD.print(F("Plik:"));
      LCD.setCursor(0, 1);
      LCD.print(fileName);
      break;
    }
  }

  for(uint8_t i = 0; i < sensorCount; i++) {
    sensors.getAddress(sensorAddress[i], i);
    sensors.setResolution(sensorAddress[i], SENSOR_RES);
  }

  delay(5000);
  LCD.clear();
  LCD.print(F("Temperatura"));
}

void loop() {
  static uint8_t loopCounter = 0;
  static uint32_t writeCounter = 0;
  sensors.requestTemperatures();

  for(uint8_t i = 0; i < sensorCount; i++) {
    float temp = sensors.getTempC(sensorAddress[i]);
    LCD.setCursor(0, 1);
    LCD.print(i + 1, DEC);
    LCD.print(F(": "));

    LCD.print(temp, 4);
    LCD.print(F("\xdf" "C"));

    if(sensorRead[i] != 0) {
      sensorRead[i] = (sensorRead[i] + temp) / 2;
    } else {
      sensorRead[i] = temp;
    }

    delay(3000);
  }

  //Save to file every 20 cycles
  if(++loopCounter == 20) {
      // Show "write" icon
      LCD.setCursor(15, 0);
      LCD.write(0xd0);

      File dataFile = SD.open(fileName, FILE_WRITE);
      dataFile.print(String(writeCounter, DEC));
      dataFile.print(";");
      for(uint8_t i = 0; i < sensorCount; i++) {
        dataFile.print(String(sensorRead[i], 4));
        if(i + 1 < sensorCount) dataFile.print(F(";"));
      }
      dataFile.print(F("\n"));
      dataFile.close();

      writeCounter++;
      loopCounter = 0;
      for(uint8_t i = 0; i < sensorCount; i++) {
        sensorRead[i] = 0;
      }

      // Hide "write" icon
      LCD.setCursor(15, 0);
      LCD.write(' ');
    }

}
