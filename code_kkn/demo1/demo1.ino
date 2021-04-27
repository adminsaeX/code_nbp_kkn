//================Library=====================//
#include <ESP8266WiFi.h>
#include <ModbusRtu.h>

//================Set up Wifi ===============//
#define SSID "SAE_DEMO"
#define SSID_PASSWORD "qwerty987654"

//===============Data======================//

uint16_t au16data[16];
uint8_t u8state;
uint8_t u8query;

Modbus master(0,1,0);
modbus_t telegram[8];
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
