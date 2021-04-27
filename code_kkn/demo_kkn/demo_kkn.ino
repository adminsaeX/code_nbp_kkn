//================Library=====================//
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ModbusRtu.h>

//================Set up Wifi ===============//
#ifndef STASSID
#define STASSID "TP-LINK_campus power"
#define STAPSK  "034257218"
#endif

unsigned int localPort = 8015; // port 
//================Modbus ================//
uint16_t au16data[16];
uint8_t u8state;
uint8_t u8query;
unsigned long u32wait;
unsigned long last;
Modbus master(0,0,0);
modbus_t telegram[10];

//================Packet===============//


WiFiUDP Udp;
IPAddress ipserver(178,128,219,211);
char Buffer[] = "";
int  str_len = 0;
int Volt = 0;
int Amp = 0; 
int Watt = 0;
int StatusRun = 0;
int Temp = 0;
int Alarm = 0;
int Digital = 0;
int Frequency = 0;
int Status = 0;
//==============Define device==========//

String deviceID = "KKN1201900" ;// 8 digit
String projectID = "KKN1" ;// 4 digit
String api_key = "KKN12019";


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(500);
  }
   digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
  Udp.begin(localPort);
  master.begin( 9600 ); // baud-rate at 19200
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = u8query = 0;  

   // Status
  telegram[0].u8id = 1; 
  telegram[0].u8fct = 3; 
  telegram[0].u16RegAdd = 50; 
  telegram[0].u16CoilsNo = 1; 
  telegram[0].au16reg = au16data; 

  // Voltage bus 
  telegram[1].u8id = 1; 
  telegram[1].u8fct = 3; 
  telegram[1].u16RegAdd = 153; 
  telegram[1].u16CoilsNo = 1; 
  telegram[1].au16reg = au16data+1; 

  // Current
  telegram[2].u8id = 1; 
  telegram[2].u8fct = 3; 
  telegram[2].u16RegAdd = 154; 
  telegram[2].u16CoilsNo = 1; 
  telegram[2].au16reg = au16data+2; 

  // Power
  telegram[3].u8id = 1; 
  telegram[3].u8fct = 3;
  telegram[3].u16RegAdd = 156; 
  telegram[3].u16CoilsNo = 1; 
  telegram[3].au16reg = au16data+3; 

  // Temperature
  telegram[4].u8id = 1; 
  telegram[4].u8fct = 3; 
  telegram[4].u16RegAdd = 157; 
  telegram[4].u16CoilsNo = 1; 
  telegram[4].au16reg = au16data+4; 
  // Status 
  telegram[5].u8id = 1; 
  telegram[5].u8fct = 3; 
  telegram[5].u16RegAdd = 160; 
  telegram[5].u16CoilsNo = 1; 
  telegram[5].au16reg = au16data+5; 

  // Alarm
  telegram[6].u8id = 1; 
  telegram[6].u8fct = 3; 
  telegram[6].u16RegAdd = 161; 
  telegram[6].u16CoilsNo = 1; 
  telegram[6].au16reg = au16data+6; 
  // digital 
  telegram[7].u8id = 1; 
  telegram[7].u8fct = 3; 
  telegram[7].u16RegAdd = 210; 
  telegram[7].u16CoilsNo = 1; 
  telegram[7].au16reg = au16data+7; 
  // frequncy 
  telegram[8].u8id = 1; 
  telegram[8].u8fct = 3; 
  telegram[8].u16RegAdd = 155; 
  telegram[8].u16CoilsNo = 1; 
  telegram[8].au16reg = au16data+8; 
  
}

void loop() {
   switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    master.query( telegram[u8query] ); // send query (only once)
    u8state++;
    u8query++;
  if (u8query >9) u8query = 0;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 200; 
    }
    break;
  }

  
  String message =  "{\"api_key\":\""+api_key+"\","
                   +"\"dev_ID\":\""+deviceID+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"data\":[{\"dcV\":\""+Volt+"\","
                   +"\"dcA\":\""+Amp+"\","
                   +"\"dcW\":\""+Watt+"\","
                   +"\"temp\":\""+Temp+"\","
                   +"\"frequency\":\""+Frequency+"\","
                   +"\"status_run\":\""+StatusRun+"\","
                   +"\"status\":\""+Status+"\","
                   +"\"alarm\":\""+Alarm+"\"}],}";


 str_len = message.length()+1;
  Buffer[str_len];
 message.toCharArray(Buffer,str_len);

  
  if(millis()-last>5000)
{
  
 last = millis();
  Udp.beginPacket(ipserver, Udp.remotePort());
  Udp.write(Buffer);
  Udp.endPacket();
  
}

}
