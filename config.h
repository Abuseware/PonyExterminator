//RF24 config
#define RF_CHANNEL 1
#define RF_NODE 1

#define RF_PIN_CE 7
#define RF_PIN_CS 8

//High for MCP9700, low for DS18B20
#define SELECTOR_PIN 2

//MCP9700A config
#define MCP_PIN A0
#define MCP_CALIBRATION -1.5L
#define MCP_SAMPLES 200

//DS18B20 config
#define ONEWIRE_PIN 5
#define DS_SENSOR_RES 12

//Power supply watch config
#define VCC_PIN A2
#define VCC_LOW 2.7L
#define VCC_HIGH 4.2L
#define VCC_SAMPLES 100
