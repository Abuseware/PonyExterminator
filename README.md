# Arduino wireless temperature sensor

## Description
This code will work with Maxim DS18B20 or Microchip MCP9700A sensors, selection
is done on boot via IO pin.

While DS18B20 is better choice for stability of readings, it suffers from speed
and power demands, especially minimum supply voltage. MCP9700A is almost 5
times cheaper, and readings via built-in ADC should be very fast, even with
multisampling enabled which provides lower resolution in comparision to DS18B20,
but comparable in stability results.

Designed for Arduino Pro or Pro Mini, but will work with any ATmega328p based
Arduino.

## Configuration
All options are available in config.h file.

### General

#### SELECTOR_PIN
Which pin is used to select between Maxim and Microchip sensors. Default: 2.

### MCP9700A

#### MCP_PIN
Analog pin connected to MCP9700A sensor, default is A0

#### MCP_CALIBRATION
Microchip sensor isn't calibrated well, add offset based on external readings
or use Maxims chip for this. Default: -1.5L

#### MCP_SAMPLES
Define number of samples per reading, keep it in range of 1-255. If samples
are above 1 then result will be median of all samples. Default: 200

### DS18B20

#### ONEWIRE_PIN
Pin for 1Wire protocol used by this IC, default is 5

#### DS_SENSOR_RES
Resolution of readings in range from 9 to 12 bits. Default: 12

### POWER

#### VCC_PIN
This pin is used to read voltage level of power supply. Default board
is designed to work in range up to 4.4V for use with Li-Ion/Li-Po batteries.

#### VCC_LOW
Voltage level which should be used as "discharged"

#### VCC_HIGH
Voltage level which should be used as 100%

#### VCC_SAMPLES
Read charge as median of VCC_SAMPLES samples.

### NETWORK

#### RF_CHANNE
Radio channel

#### RF_NODE
Radio node id (1-5)

#### RF_PIN_CE
Pin connected to nRF24l01 CE

#### RF_PIN_CS
Pin connected to nRF24l01 CSN

