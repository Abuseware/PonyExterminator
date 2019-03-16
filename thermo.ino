/* Digital thermometer by Artur "Licho" Kaleta
 *
 * Few things first:
 * You can use DS18B20 or MCP9700A and select proper device by providing LOW or HIGH on SELECTOR_PIN
 * MCP9700A version is cheaper, faster and works on lower voltages which is good for battery powered devices, but with lower precision in readings
 * DS18B20 requires AT LEAST 3V to work, also is very slow, especially with precision bumped to 12bits
 */
 
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Adafruit_SleepyDog.h>

#include "config.h"
#if MCP_SAMPLES > 255 || MCP_SAMPLES < 1
  #error MCP_SAMPLES is out of range (1-255)
#endif

RF24 radio(7,8);
RF24Network network(radio);

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

uint8_t mode;

int compareInt(const void* a, const void* b) {
  return (*(int*)a - *(int*)b);
}

float getTemperature() {
  float temp;

  //MCP9700A
  if(mode) {
    /*
     * Calculations:
     * Multiply ADC value by ADC reference voltage
     * Divide by available steps (1024 for 10bit)
     * Substract 0.5V which is 0C point
     * Divide by 0.01V (10mV per step)
     */

    #if MCP_SAMPLES > 1
      int results[MCP_SAMPLES];
      for(uint8_t i = 0; i < MCP_SAMPLES; i++) {
        results[i] = analogRead(MCP_PIN);
        //delay(5);
      }
      qsort(results, MCP_SAMPLES, sizeof(int), compareInt);
      temp = (((results[(MCP_SAMPLES / 2) - 1] * 1.1L) / 1024.0L) - 0.5L) / 0.01L;
    #else
      temp = (((analogRead(MCP_PIN) * 1.1L) / 1024.0L) - 0.5L) / 0.01L;
    #endif
    temp += MCP_CALIBRATION; //Calibration value

  //DS18B20
  } else {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
  }

  return temp;
}

float getVoltage() {
  int results[VCC_SAMPLES];
  for(uint8_t i = 0; i < VCC_SAMPLES; i++) {
    results[i] = analogRead(VCC_PIN);
  }
  qsort(results, VCC_SAMPLES, sizeof(int), compareInt);
  return ((float)results[(VCC_SAMPLES / 2) - 1] / 1023L) * 4.3L;
}

int getCharge() {
  return  100 * ((getVoltage() - VCC_LOW) / (VCC_HIGH - VCC_LOW));
}

void setup() {
  SPI.begin();
  radio.begin();
  radio.setChannel(RF_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(1, 15);
  network.begin(RF_NODE);
  
  

  pinMode(SELECTOR_PIN, INPUT);
  mode = digitalRead(SELECTOR_PIN);


  analogReference(INTERNAL); //Use 1v1 AREF which limits us to ~50C readings, fair enough.

  // SELECTOR_PIN high - use MCP9700A via ADC
  if(mode) {
    for(uint8_t i = 0; i < 100; i++) analogRead(MCP_PIN); //First few readings should be discarded - see datasheet

  // SELECTOR_PIN low - use DS18B20 via 1Wire
  } else {
    sensors.begin();
    if(sensors.getDeviceCount() != 1) {
      while(true);
    }

    sensors.setResolution(DS_SENSOR_RES);
  }

}

void loop() {
  network.update();

  while(network.available()){
    RF24NetworkHeader header;
    float value;
    network.read(header, &value, sizeof(value));
    if(header.from_node == 0) {
      RF24NetworkHeader tx_header(header.from_node, header.type);
      float tx_value = -1337;
      switch(header.type - 65){
        case 0:
          tx_value = getTemperature();
          break;
        case 1:
          tx_value = getVoltage();
          break;
        case 2:
          tx_value = getCharge();
          break;
      }

      if(tx_value != -1337){
        for(uint8_t i = 0; i < 10; i++){
          if(network.write(tx_header, &tx_value, sizeof(tx_value))) break;
        }
      }
    }
  }
  Watchdog.sleep(1000);
}
