#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <RTClib.h>

#include <SPI.h>
#include <SD.h>

#define ONEWIRE_PIN 2
#define CS_PIN 10

#define SENSOR_RES 12

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

RTC_DS1307 RTC;

uint8_t lastCheck = 60;

void setup() {
  while(!Serial);
  Serial.begin(9600);

  Serial.print(F("Initializing SD card..."));

  // see if the card is present and can be initialized:
  if (!SD.begin(CS_PIN)) {
    Serial.println(F(" failed, or not present."));
    // don't do anything more:
    while (1);
  }
  Serial.println(F(" card initialized."));

  Wire.begin();
  RTC.begin();

  if(!RTC.isrunning())
   {
      Serial.println(F("RTC is NOT running!"));
      // following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(__DATE__,__TIME__));
   }

  sensors.begin();
  sensors.setResolution(SENSOR_RES);

  lastCheck = RTC.now().minute();
}

void loop() {
  DateTime now = RTC.now();
  if(lastCheck != now.minute()) {
    lastCheck = now.minute();

    sensors.requestTemperatures();

    String buf = String(now.unixtime(), DEC) + ";" + String(sensors.getTempCByIndex(0), 4);

    File dataFile = SD.open("temp.csv", FILE_WRITE);
    if(dataFile) {
      if(dataFile.position() == 0) {
        Serial.println(F("New file, writing header."));
        Serial.println(F("Timestamp;Degrees"));
        dataFile.println(F("Timestamp;Degrees"));
      }
      dataFile.println(buf);
      dataFile.close();
      Serial.println(buf);
    } else {
      Serial.println(F("Error while writing log file!"));
    }
  }

  delay(5000);
}
