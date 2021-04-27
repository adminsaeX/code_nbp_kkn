//================Library=====================//
#include <HTTPClient.h>
#include <WiFi.h>
#include <ModbusRtu.h>


//================Set up Wifi ===============//
#define WIFI_STA_NAME "SAE"
#define WIFI_STA_PASS "qwerty987654"

//================Modbus ================//
uint16_t au16data[16];
uint8_t u8state;
uint8_t u8query;
unsigned long u32wait;
unsigned long last;
unsigned long interval = 60000;
Modbus master(0,0,0);
modbus_t telegram[20];

int Volt = 0;
int Amp = 0; 
int Watt = 0;
int StatusRun = 0;
int Temp = 0;
int Alarm = 0;
int Digital = 0;
int Frequency = 0;
int Status = 0;
String payload = "";
int led_g = 26 ;
int led_r = 27;
//==============Define device==========//

String deviceID = "KKN1201908" ;// 8 digit
String projectID = "KKN1" ;// 4 digit
String api_key = "KKN12019";

void setup() {
  pinMode(led_g,OUTPUT);
  pinMode(led_r,OUTPUT);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led_r,HIGH);
    delay(300);
    digitalWrite(led_r,LOW);
    delay(300);
    
  }
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
  digitalWrite(led_r,HIGH);
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
    digitalWrite(led_g,HIGH);
    delay(300);
    digitalWrite(led_g,LOW);
    delay(300);
    }
    break;
  }

  Volt = au16data[1];
  Amp  = au16data[2];
  Watt = au16data[3];
  Temp = au16data[4];
  Alarm  = au16data[6];
  Digital = au16data[7];
  Frequency = au16data[8];
  StatusRun = au16data[0];
  Status = au16data[5];

  
  payload = "pro_ID="+projectID+"&dev_ID="+deviceID+"&V="+String(Volt)+"&A="+String(Amp)+"&W="+String(Watt)
             +"&temp="+String(Temp)+"&alarm="+String(Alarm)+"&digital="+String(Digital)+"&freq="+String(Frequency)+"&statusRun="+String(StatusRun)
             +"&status="+String(Status);
  if(millis()-last>interval)
  {
    sendData(payload);
    last = millis();
  }

  
}
void sendData(String payload)
{
  String url = "http://178.128.219.211:9010/data";
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  int httpCode = http.POST(payload);
  http.end();
 
  
}
