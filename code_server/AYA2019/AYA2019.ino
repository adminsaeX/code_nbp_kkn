//=============================Library======================//
#include <AIS_NB_BC95.h>
#include <DHT.h>

#include <EEPROM.h>
#include <avr/wdt.h>
#include <ModbusRtu.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//==========================Define Server ==================//
String apnName = "devkit.nb";
String serverIP = "178.128.219.211"; // Server IP 
String serverPort = "8011"; // Server port
AIS_NB_BC95 AISnb;

//=========================Define device ===================//
String deviceID1 = "AYA201901" ;// 8 digit
String deviceID2 = "AYA201902" ;// 8 digit
String deviceID3 = "AYA201903" ;// 8 digit
String deviceID4 = "AYA201904" ;// 8 digit
String deviceID5 = "AYA201905" ;// 8 digit
String deviceID6 = "AYA201906" ;// 8 digit
String deviceID7 = "AYA201907" ;// 8 digit
String deviceID8 = "AYA201908" ;// 8 digit
String deviceID9 = "AYA201909" ;// 8 digit
String deviceID10 = "AYA201910" ;// 8 digit
String deviceID00 = "AYA201900" ;// 8 digit
String projectID = "AYA1" ;// 4 digit
String api_key = "12345678";

#define DEBUG_Board  false
//=========================Define Variable system =========//
const long interval_send = 60000 ; // time to send data genaral millisecond
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long interval_calFlow = 1000 ;// time cal Flow
unsigned long preCal = 0;
unsigned long currentCal = 0;
unsigned long interval_power = 1000; // time cal power
unsigned long preCal_power =0;
unsigned long currentCal_power =0 ;

#define ONE_WIRE_BUS 9

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature TempBoard(&oneWire);
//=========================Define I/O sensor===============//
#define DHTPIN 10
#define DHTTYPE DHT22
DHT dht(DHTPIN,DHTTYPE);
byte FlowPin1 = 2 ;// flow pin
byte FlowPin2 = 3 ;// flow pin
byte PV_V = A0; // Volt send
byte PV_A = A1; // Amp send
//=========================Variable=======================//
int DCVolt1 = 0; // PV Volt
int DCAmp1 = 0; // PV Current
int DCWatt1 = 0; // PV Power
float DCWh1; // PV Energy
int Fault1 = 0; //alarm
int DCVolt2 = 0; // PV Volt
int DCAmp2 = 0; // PV Current
int DCWatt2 = 0; // PV Power
float DCWh2; // PV Energy
int Fault2 = 0; //alarm
int DCVolt3 = 0; // PV Volt
int DCAmp3 = 0; // PV Current
int DCWatt3 = 0; // PV Power
float  DCWh3; // PV Energy
int Fault3 = 0; //alarm
int DCVolt4 = 0; // PV Volt
int DCAmp4 = 0; // PV Current
int DCWatt4 = 0; // PV Power
float  DCWh4; // PV Energy
int Fault4 = 0; //alarm
int DCVolt5 = 0; // PV Volt
int DCAmp5 = 0; // PV Current
int DCWatt5 = 0; // PV Power
float DCWh5; // PV Energy
int Fault5 = 0; //alarm
int DCVolt6 = 0; // PV Volt
int DCAmp6 = 0; // PV Current
int DCWatt6 = 0; // PV Power
float DCWh6; // PV Energy
int Fault6 = 0; //alarm
int DCVolt7 = 0; // PV Volt
int DCAmp7 = 0; // PV Current
int DCWatt7 = 0; // PV Power
float DCWh7; // PV Energy
int Fault7 = 0; //alarm
int DCVolt8 = 0; // PV Volt
int DCAmp8 = 0; // PV Current
int DCWatt8 = 0; // PV Power
float DCWh8; // PV Energy
int Fault8 = 0; //alarm
int DCVolt9 = 0; // PV Volt
int DCAmp9 = 0; // PV Current
int DCWatt9 = 0; // PV Power
float DCWh9; // PV Energy
int Fault9 = 0; //alarm
int DCVolt10 = 0; // PV Volt
int DCAmp10 = 0; // PV Current
int DCWatt10 = 0; // PV Power
float DCWh10; // PV Energy
int Fault10 = 0; //alarm
int Temp =0 ;
int Hum =0;


///////////////////////////////////////////////////////////////
float t=0; // get temp DHT22
float h=0; // get hum DHT22
float tempB = 0; // get DS18B20
int flow_count = 0;
int flow_test;
//////////////////////////Power meter ////////////////////////
int volt = 0;
int amp = 0;




//=========================MODBUS=========================//
uint16_t au16data[70];
uint8_t u8state;
uint8_t u8query;
/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus master(0,1,0); // Serial1 RX1 TX1

modbus_t telegram[60];

unsigned long u32wait;
/////////////inverter BPD invt////////////////////////
byte ID_hmi= 100 ;
uint16_t addDcVolt = 4609 ;
uint16_t addDcAmp = 4614;
uint16_t addDcAmp2 = 4615;
uint16_t addDcPower = 4617  ;
uint16_t addHz = 12288;
uint16_t addRPM = 12293;
uint16_t addState = 8448 ;
uint16_t addFault = 8450;
uint16_t addOutvolt = 12291;
uint16_t addOutamp = 12292;
uint16_t addOutpower = 12294;
///////////////Temp&Hum modbus////////////////////
byte ID_TempHum = 2 ;
uint16_t addTemp = 1;
uint16_t addHum = 0;

//=========================EEPROM=========================//
int e1 = 0; // byte 0 - 7
int e2 = 4;
int e3 = 8;
int e4 = 12;
int e5 = 16;
int e6 = 20;
int e7 = 24;
int e8 = 28;
int e9 = 32;
int e10 = 36;


//========================JSON Command===========================//
String command = "101"; // Genaral Command
String commandArray = "0000";



//
//
//
//


byte led_g = 6;
byte led_r = 7;


void setup() {
  //wdt_enable(WDTO_8S);
  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,LOW);
  pinMode(led_g,OUTPUT);
  pinMode(led_r,OUTPUT);
  AISnb.debug = true;
  digitalWrite(led_r,LOW);
  AISnb.setupDevice(serverPort);

 
  String ip1 = AISnb.getDeviceIP();
  delay(1000);

  pingRESP pingR = AISnb.pingIP(serverIP);
  
  signal sig = AISnb.getSignal();
  
  previousMillis = millis();

  pinMode(FlowPin1,INPUT_PULLUP);
  //attachInterrupt(FlowPin1,pulseCount,FALLING); // flow count
  //QTotal = readEEPROM(water_total);

  
  TempBoard.begin();
  master.begin(9600);
  master.setTimeOut(2000);
  u32wait = millis() + 500;
  u8state = 0;
  u8query = 0;

  telegram[0].u8id = ID_hmi; 
  telegram[0].u8fct = 3; 
  telegram[0].u16RegAdd = 11; 
  telegram[0].u16CoilsNo = 1; 
  telegram[0].au16reg = au16data; // V1
  
  telegram[1].u8id = ID_hmi; 
  telegram[1].u8fct = 3; 
  telegram[1].u16RegAdd = 12; 
  telegram[1].u16CoilsNo = 1; 
  telegram[1].au16reg = au16data+1; // V2

  telegram[2].u8id = ID_hmi; 
  telegram[2].u8fct = 3; 
  telegram[2].u16RegAdd = 13; 
  telegram[2].u16CoilsNo = 1; 
  telegram[2].au16reg = au16data+2; // v3

  telegram[3].u8id = ID_hmi; 
  telegram[3].u8fct = 3; 
  telegram[3].u16RegAdd = 14; 
  telegram[3].u16CoilsNo = 1; 
  telegram[3].au16reg = au16data+3; // v4

  telegram[4].u8id = ID_hmi; 
  telegram[4].u8fct = 3; 
  telegram[4].u16RegAdd = 15; 
  telegram[4].u16CoilsNo = 1; 
  telegram[4].au16reg = au16data+4; // v5

  telegram[5].u8id = ID_hmi; 
  telegram[5].u8fct = 3; 
  telegram[5].u16RegAdd = 16; 
  telegram[5].u16CoilsNo = 1; 
  telegram[5].au16reg = au16data+5; // v6

 telegram[6].u8id = ID_hmi; 
  telegram[6].u8fct = 3; 
  telegram[6].u16RegAdd = 17; 
  telegram[6].u16CoilsNo = 1; 
  telegram[6].au16reg = au16data+6; // v7

 telegram[7].u8id = ID_hmi; 
  telegram[7].u8fct = 3; 
  telegram[7].u16RegAdd = 18; 
  telegram[7].u16CoilsNo = 1; 
  telegram[7].au16reg = au16data+7; // v8

 telegram[8].u8id = ID_hmi; 
  telegram[8].u8fct = 3; 
  telegram[8].u16RegAdd = 19; 
  telegram[8].u16CoilsNo = 1; 
  telegram[8].au16reg = au16data+8; // v9

 telegram[9].u8id = ID_hmi; 
  telegram[9].u8fct = 3; 
  telegram[9].u16RegAdd = 20; 
  telegram[9].u16CoilsNo = 1; 
  telegram[9].au16reg = au16data+9; // v10

 telegram[10].u8id = ID_hmi; 
  telegram[10].u8fct = 3; 
  telegram[10].u16RegAdd = 21; 
  telegram[10].u16CoilsNo = 1; 
  telegram[10].au16reg = au16data+10; // c1

 telegram[11].u8id = ID_hmi; 
  telegram[11].u8fct = 3; 
  telegram[11].u16RegAdd = 22; 
  telegram[11].u16CoilsNo = 1; 
  telegram[11].au16reg = au16data+11; // c2

 telegram[12].u8id = ID_hmi; 
  telegram[12].u8fct = 3; 
  telegram[12].u16RegAdd = 23; 
  telegram[12].u16CoilsNo = 1; 
  telegram[12].au16reg = au16data+12; // c3

 telegram[13].u8id = ID_hmi; 
  telegram[13].u8fct = 3; 
  telegram[13].u16RegAdd = 24; 
  telegram[13].u16CoilsNo = 1; 
  telegram[13].au16reg = au16data+13; // c4

 telegram[14].u8id = ID_hmi; 
  telegram[14].u8fct = 3; 
  telegram[14].u16RegAdd = 25; 
  telegram[14].u16CoilsNo = 1; 
  telegram[14].au16reg = au16data+14; // c5

 telegram[15].u8id = ID_hmi; 
  telegram[15].u8fct = 3; 
  telegram[15].u16RegAdd = 26; 
  telegram[15].u16CoilsNo = 1; 
  telegram[15].au16reg = au16data+15; // c6

 telegram[16].u8id = ID_hmi; 
  telegram[16].u8fct = 3; 
  telegram[16].u16RegAdd = 27; 
  telegram[16].u16CoilsNo = 1; 
  telegram[16].au16reg = au16data+16; // c7

 telegram[17].u8id = ID_hmi; 
  telegram[17].u8fct = 3; 
  telegram[17].u16RegAdd = 28; 
  telegram[17].u16CoilsNo = 1; 
  telegram[17].au16reg = au16data+17; // c8

 telegram[18].u8id = ID_hmi; 
  telegram[18].u8fct = 3; 
  telegram[18].u16RegAdd = 29; 
  telegram[18].u16CoilsNo = 1; 
  telegram[18].au16reg = au16data+18; // c9
 telegram[19].u8id = ID_hmi; 
  telegram[19].u8fct = 3; 
  telegram[19].u16RegAdd = 30; 
  telegram[19].u16CoilsNo = 1; 
  telegram[19].au16reg = au16data+19; // c10

 telegram[20].u8id = ID_hmi; 
  telegram[20].u8fct = 3; 
  telegram[20].u16RegAdd = 31; 
  telegram[20].u16CoilsNo = 1; 
  telegram[20].au16reg = au16data+20; // p1

 telegram[21].u8id = ID_hmi; 
  telegram[21].u8fct = 3; 
  telegram[21].u16RegAdd = 32; 
  telegram[21].u16CoilsNo = 1; 
  telegram[21].au16reg = au16data+21; // p2

 telegram[22].u8id = ID_hmi; 
  telegram[22].u8fct = 3; 
  telegram[22].u16RegAdd = 33; 
  telegram[22].u16CoilsNo = 1; 
  telegram[22].au16reg = au16data+22; // p3

 telegram[23].u8id = ID_hmi; 
  telegram[23].u8fct = 3; 
  telegram[23].u16RegAdd = 34; 
  telegram[23].u16CoilsNo = 1; 
  telegram[23].au16reg = au16data+23; // p4

 telegram[24].u8id = ID_hmi; 
  telegram[24].u8fct = 3; 
  telegram[24].u16RegAdd = 35; 
  telegram[24].u16CoilsNo = 1; 
  telegram[24].au16reg = au16data+24; // p5

 telegram[25].u8id = ID_hmi; 
  telegram[25].u8fct = 3; 
  telegram[25].u16RegAdd = 36; 
  telegram[25].u16CoilsNo = 1; 
  telegram[25].au16reg = au16data+25; // p6

 telegram[26].u8id = ID_hmi; 
  telegram[26].u8fct = 3; 
  telegram[26].u16RegAdd = 37; 
  telegram[26].u16CoilsNo = 1; 
  telegram[26].au16reg = au16data+26; // p7

 telegram[27].u8id = ID_hmi; 
  telegram[27].u8fct = 3; 
  telegram[27].u16RegAdd = 38; 
  telegram[27].u16CoilsNo = 1; 
  telegram[27].au16reg = au16data+27; // p8

 telegram[28].u8id = ID_hmi; 
  telegram[28].u8fct = 3; 
  telegram[28].u16RegAdd = 39; 
  telegram[28].u16CoilsNo = 1; 
  telegram[28].au16reg = au16data+28; // p9

 telegram[29].u8id = ID_hmi; 
  telegram[29].u8fct = 3; 
  telegram[29].u16RegAdd = 40; 
  telegram[29].u16CoilsNo = 1; 
  telegram[29].au16reg = au16data+29; // p10

 telegram[30].u8id = ID_hmi; 
  telegram[30].u8fct = 3; 
  telegram[30].u16RegAdd = 41; 
  telegram[30].u16CoilsNo = 1; 
  telegram[30].au16reg = au16data+30; // e1
  
 telegram[31].u8id = ID_hmi; 
  telegram[31].u8fct = 3; 
  telegram[31].u16RegAdd = 42; 
  telegram[31].u16CoilsNo = 1; 
  telegram[31].au16reg = au16data+31; // e2

 telegram[32].u8id = ID_hmi; 
  telegram[32].u8fct = 3; 
  telegram[32].u16RegAdd = 43; 
  telegram[32].u16CoilsNo = 1; 
  telegram[32].au16reg = au16data+32; // e3

 telegram[33].u8id = ID_hmi; 
  telegram[33].u8fct = 3; 
  telegram[33].u16RegAdd = 44; 
  telegram[33].u16CoilsNo = 1; 
  telegram[33].au16reg = au16data+33; // e4

 telegram[34].u8id = ID_hmi; 
  telegram[34].u8fct = 3; 
  telegram[34].u16RegAdd = 45; 
  telegram[34].u16CoilsNo = 1; 
  telegram[34].au16reg = au16data+34; // e5

 telegram[35].u8id = ID_hmi; 
  telegram[35].u8fct = 3; 
  telegram[35].u16RegAdd = 46; 
  telegram[35].u16CoilsNo = 1; 
  telegram[35].au16reg = au16data+35; // e6

 telegram[36].u8id = ID_hmi; 
  telegram[36].u8fct = 3; 
  telegram[36].u16RegAdd = 47; 
  telegram[36].u16CoilsNo = 1; 
  telegram[36].au16reg = au16data+36; // e7

 telegram[37].u8id = ID_hmi; 
  telegram[37].u8fct = 3; 
  telegram[37].u16RegAdd = 48; 
  telegram[37].u16CoilsNo = 1; 
  telegram[37].au16reg = au16data+37; // e8

 telegram[38].u8id = ID_hmi; 
  telegram[38].u8fct = 3; 
  telegram[38].u16RegAdd = 49; 
  telegram[38].u16CoilsNo = 1; 
  telegram[38].au16reg = au16data+38; // e9

 telegram[39].u8id = ID_hmi; 
  telegram[39].u8fct = 3; 
  telegram[39].u16RegAdd = 50; 
  telegram[39].u16CoilsNo = 1; 
  telegram[39].au16reg = au16data+39; // e10
  
  telegram[40].u8id = ID_hmi; 
  telegram[40].u8fct = 3; 
  telegram[40].u16RegAdd = 51; 
  telegram[40].u16CoilsNo = 1; 
  telegram[40].au16reg = au16data+40; // A1

  telegram[41].u8id = ID_hmi; 
  telegram[41].u8fct = 3; 
  telegram[41].u16RegAdd = 52; 
  telegram[41].u16CoilsNo = 1; 
  telegram[41].au16reg = au16data+41; // A2

  telegram[42].u8id = ID_hmi; 
  telegram[42].u8fct = 3; 
  telegram[42].u16RegAdd = 53; 
  telegram[42].u16CoilsNo = 1; 
  telegram[42].au16reg = au16data+42; // A3

  telegram[43].u8id = ID_hmi; 
  telegram[43].u8fct = 3; 
  telegram[43].u16RegAdd = 54; 
  telegram[43].u16CoilsNo = 1; 
  telegram[43].au16reg = au16data+43; // A4

  telegram[44].u8id = ID_hmi; 
  telegram[44].u8fct = 3; 
  telegram[44].u16RegAdd = 55; 
  telegram[44].u16CoilsNo = 1; 
  telegram[44].au16reg = au16data+44; // A5

  telegram[45].u8id = ID_hmi; 
  telegram[45].u8fct = 3; 
  telegram[45].u16RegAdd = 56; 
  telegram[45].u16CoilsNo = 1; 
  telegram[45].au16reg = au16data+45; // A6

  telegram[46].u8id = ID_hmi; 
  telegram[46].u8fct = 3; 
  telegram[46].u16RegAdd = 57; 
  telegram[46].u16CoilsNo = 1; 
  telegram[46].au16reg = au16data+46; // A7
 
  telegram[47].u8id = ID_hmi; 
  telegram[47].u8fct = 3; 
  telegram[47].u16RegAdd = 58; 
  telegram[47].u16CoilsNo = 1; 
  telegram[47].au16reg = au16data+47; // A8

  telegram[48].u8id = ID_hmi; 
  telegram[48].u8fct = 3; 
  telegram[48].u16RegAdd = 59; 
  telegram[48].u16CoilsNo = 1; 
  telegram[48].au16reg = au16data+48; // A9

  telegram[49].u8id = ID_hmi; 
  telegram[49].u8fct = 3; 
  telegram[49].u16RegAdd = 60; 
  telegram[49].u16CoilsNo = 1; 
  telegram[49].au16reg = au16data+49; // A10

  
 
 
 
  
  digitalWrite(led_r,HIGH);
  
  //dht.begin();
  
}

void loop() {

//==============================MODBUS Query=======================//
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    master.query( telegram[u8query] ); // send query (only once)
    u8state++;
    u8query++;
  if (u8query > 51) u8query = 0;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 500;
      digitalWrite(led_g,HIGH);
      delay(200);
      digitalWrite(led_g,LOW);
      delay(100); 
    }
    break;
  }

  DCVolt1 = au16data[0];
  DCAmp1 =   au16data[10];
  DCWatt1 =  au16data[20];
  DCWh1  = au16data[30];
  Fault1 =   au16data[40];

  DCVolt2 = au16data[1];
  DCAmp2 =   au16data[11];
  DCWatt2 =  au16data[21];
  DCWh2 = au16data[31];
  Fault2 =   au16data[41];

  DCVolt3 = au16data[2];
  DCAmp3 =   au16data[12];
  DCWatt3 =  au16data[22];
  DCWh3 = au16data[32];
  Fault3 =   au16data[42];
  
  DCVolt4 = au16data[3];
  DCAmp4 =   au16data[13];
  DCWatt4 =  au16data[23];
  DCWh4 = au16data[33];
  Fault4 =   au16data[43];

  DCVolt5 = au16data[4];
  DCAmp5 =   au16data[14];
  DCWatt5 =  au16data[24];
  DCWh5 = au16data[34];
  Fault5 =   au16data[44];

  DCVolt6 = au16data[5];
  DCAmp6 =   au16data[15];
  DCWatt6 =  au16data[25];
  DCWh6 = au16data[35];
  Fault6 =   au16data[45];

  DCVolt7 = au16data[6];
  DCAmp7 =   au16data[16];
  DCWh7 = au16data[36];
  Fault7 =   au16data[46];

  DCVolt8 = au16data[7];
  DCAmp8 =   au16data[17];
  DCWatt8 =  au16data[27];
  DCWh8 = au16data[37];
  Fault8 =   au16data[47];
  
  DCVolt9 = au16data[8];
  DCAmp9 =   au16data[18];
  DCWatt9 =  au16data[28];
  DCWh9 = au16data[38];
  Fault9 =   au16data[48];

  DCVolt10 = au16data[9];
  DCAmp10 =   au16data[19];
  DCWatt10 =  au16data[29];
  DCWh10 = au16data[39];
  Fault10 =   au16data[49];
  
  t = dht.readTemperature();
  h = dht.readHumidity();

  Temp = t*10.0;
  Hum  = h*10.0;
  
//  TempBoard.requestTemperatures();
 // tempB = TempBoard.getTempCByIndex(0);
  tempB =0 ;

 
 

//======================JSON Data==========================//
/*
 String message = "{\"deviceID\":\""+deviceID+"\","
                  +"\"projectID\":\""+projectID+"\","
                  +"\"command\":\""+command+"\","
                  +"\"comArray\":\""+commandArray+"\","
                  +"\"data\":["
                  +""+DCVolt+","
                  +""+DCAmp+","
                  +""+DCWatt+","
                  +""+DCWh+","
                  +""+ACVolt+","
                  +""+ACAmp+","
                  +""+ACWatt+","
                  +""+ACWh+","
                  +""+FlowRate+","
                  +""+QTotal+","
                  +""+Temp+","
                  +""+Hum+","
                  +""+Fault+","
                  +""+PumpState+","
                  +""+PumpHz+","
                  +""+DigitalIn+","
                  +""+OutVolt+","
                  +""+OutAmp+","
                  +""+OutPower+","
                  +""+RPM+","
                  +""+solarEnergy+","
                  +""+PVTemp+","
                  +" \"crc\":\""+String(crc)+"\"}";
*/
String message01 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID1+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt1+"\","
                   +"\"dcA\":\""+DCAmp1+"\","
                   +"\"dcW\":\""+DCWatt1+"\","
                   +"\"dcWh\":\""+DCWh1+"\","
                   +"\"fault\":\""+Fault1+"\"}]}";
String message02 =  "{\"api_key\":\""+api_key+"\","
                    +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID2+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt2+"\","
                   +"\"dcA\":\""+DCAmp2+"\","
                   +"\"dcW\":\""+DCWatt2+"\","
                   +"\"dcWh\":\""+DCWh2+"\","
                   +"\"fault\":\""+Fault2+"\"}]}";
                   
String message03 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID3+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt3+"\","
                   +"\"dcA\":\""+DCAmp3+"\","
                   +"\"dcW\":\""+DCWatt3+"\","
                   +"\"dcWh\":\""+DCWh3+"\","
                   +"\"fault\":\""+Fault3+"\"}]}";
                   
String message04 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID4+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt4+"\","
                   +"\"dcA\":\""+DCAmp4+"\","
                   +"\"dcW\":\""+DCWatt4+"\","
                   +"\"dcWh\":\""+DCWh4+"\","
                   +"\"fault\":\""+Fault4+"\"}]}";
                   
String message05 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID5+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt5+"\","
                   +"\"dcA\":\""+DCAmp5+"\","
                   +"\"dcW\":\""+DCWatt5+"\","
                   +"\"dcWh\":\""+DCWh5+"\","
                   +"\"fault\":\""+Fault5+"\"}]}";
                   
String message06 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID6+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt6+"\","
                   +"\"dcA\":\""+DCAmp6+"\","
                   +"\"dcW\":\""+DCWatt6+"\","
                   +"\"dcWh\":\""+DCWh6+"\","
                   +"\"fault\":\""+Fault6+"\"}]}";
                   
String message07 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID7+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt7+"\","
                   +"\"dcA\":\""+DCAmp7+"\","
                   +"\"dcW\":\""+DCWatt7+"\","
                   +"\"dcWh\":\""+DCWh7+"\","
                   +"\"fault\":\""+Fault7+"\"}]}";
                   
String message08 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID8+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt8+"\","
                   +"\"dcA\":\""+DCAmp8+"\","
                   +"\"dcW\":\""+DCWatt8+"\","
                   +"\"dcWh\":\""+DCWh8+"\","
                   +"\"fault\":\""+Fault8+"\"}]}";
                   
String message09 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID9+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt9+"\","
                   +"\"dcA\":\""+DCAmp9+"\","
                   +"\"dcW\":\""+DCWatt9+"\","
                   +"\"dcWh\":\""+DCWh9+"\","
                   +"\"fault\":\""+Fault9+"\"}]}";
                   
String message10 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID10+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt10+"\","
                   +"\"dcA\":\""+DCAmp10+"\","
                   +"\"dcW\":\""+DCWatt10+"\","
                   +"\"dcWh\":\""+DCWh10+"\","
                   +"\"fault\":\""+Fault10+"\"}]}";
           
String message00 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID00+"\","
                   +"\"data\":[{\"temp\":\""+Temp+"\","
                   +"\"hum\":\""+Hum+"\"}]}";
                               
                 
 

 // Serial.println(message);
 // delay(2000);


//String sTr = "{\"data\":[{\"dcV\":\"200\",\"dcA\":\"10\"}]}";


 //========================Send Data General ============================//
 
    currentMillis = millis();
 if(currentMillis - previousMillis >= interval_send)
 {

  String udpHex1 = AISnb.str2HexStr(message01);
  
  UDPSend udp1 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex1);
  delay(200);
  String udpHex2 = AISnb.str2HexStr(message02);
  UDPSend udp2 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex2);
  delay(200);
  String udpHex3 = AISnb.str2HexStr(message03);
  UDPSend udp3 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex3);
 delay(200);
  String udpHex4 = AISnb.str2HexStr(message04);
  UDPSend udp4 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex4);
delay(200);
  String udpHex5 = AISnb.str2HexStr(message05);
  UDPSend udp5 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex5);
delay(200);
  String udpHex6 = AISnb.str2HexStr(message06);
  UDPSend udp6 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex6);
 delay(200);
  String udpHex7 = AISnb.str2HexStr(message07);
  UDPSend udp7 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex7);
  delay(200);
  String udpHex8 = AISnb.str2HexStr(message08);
  UDPSend udp8 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex8);
  delay(200);
  String udpHex9 = AISnb.str2HexStr(message09);
  UDPSend udp9 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex9);
  delay(200);
  String udpHex10 = AISnb.str2HexStr(message10);
  UDPSend udp10 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex10);
  delay(200);
   String udpHex00 = AISnb.str2HexStr(message00);
   UDPSend udp00 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex00);
  previousMillis = currentMillis;
  
  
 }
 
 
 //wdt_reset();
 //========================Receive Data from Server ===================//
 
 UDPReceive resp = AISnb.waitResponse();
 String input = AISnb.toString(resp.data);

 if(input != "")
 {
  Serial.println(input);
  //delay(1000);
  input = "";
  
 }
 //calFlow();
  
  
 //wdt_reset();
}
/*
void calFlow()
{ 
    flow_test = random(1,3);
    currentCal = millis();
  if(currentCal - preCal >=interval_calFlow)
  {
    detachInterrupt(digitalPinToInterrupt(FlowPin1));
    FlowRate = ((interval_calFlow/1000.0)*flow_test); // Hz
    FlowRate = FlowRate*FlowScal; //L/s
    FlowRateQ = FlowRate/1000.0; //m^3/s
    FlowRate = FlowRate*3600.0; // L/hr
    FlowRateM = FlowRate/1000.0 ; // m^3/hr
    
    
    QTotal += FlowRateQ; //total m^3
    if(QTotal > 3.4E+38)
    {
       for (int i = 0 ; i < 4 ; i++) {
    EEPROM.write(water_total+i, 0);
     }
     QTotal = readEEPROM(water_total);

    }
    if(DEBUG_Board){
    Serial.print("count=");
    Serial.print(flow_test);
    Serial.print("flow=");
    Serial.print(FlowRateM);
    Serial.print("Total=");
    Serial.println(QTotal);
    }
    preCal = currentCal ;
    flow_test = 0;
    attachInterrupt(digitalPinToInterrupt(FlowPin1),pulseCount,FALLING);
    
    sumFlow++;

    if(sumFlow>20)
    {
      writeEEPROM(water_total,QTotal);
      sumFlow = 0;
    }
    
    
  }
}

 float calPower(int DCWatt,int addE)
 {
   float  DCWh;
   int sumEnergy;
   currentCal_power = millis();
   if(currentCal_power - preCal_power >=interval_power)
   {
    DCWh += (DCWatt*(interval_power/1000))/36000.0; ; //Wh
    }
    preCal_power = currentCal_power;
    
    sumEnergy++;
    
    if(sumEnergy >=20)
    {
      writeEEPROM(addE,DCWh);
      sumEnergy=0;
    }
    
    return DCWh;
   
   
 }




void EEPROMWritelong(int address, long value){

  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address){
    long four = EEPROM.read(address);
    long three = EEPROM.read(address + 1);
    long two = EEPROM.read(address + 2);
    long one = EEPROM.read(address + 3);

    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}


void pulseCount()
{
  flow_count++;
  
}

typedef union{
  float flt;
  byte array[4];
} FloatConverter;

void writeEEPROM(int address, float value)
{
  FloatConverter aConverter; //create a new variable of type FloatConverter
  aConverter.flt = value; //set its value (using the float blueprint) to the value of config
  for(byte i = 0; i < 4; i++){
    EEPROM.write(address+i,aConverter.array[i]); //store each of the 4 bytes of aConverter to the EEPROM, accessing them using the byte[4] blueprint
    //Serial.print(aConverter.array[i]);
  };
    //Serial.println();
}


//read
float readEEPROM(int address)
{
  float value;
  FloatConverter aConverter; //create a new variable of type FloatConverter
  for(byte i = 0; i < 4; i++){
    aConverter.array[i] = EEPROM.read(address+i); //read 4 bytes from the EEPROM to aConverter using the byte[4] blueprint
  }
  value = aConverter.flt; //set the value of config to the value of aConverter using the float blueprint}
  //Serial.println(value);
  return value;
}
//-------------------------------END EEPROM FUNCTION

union Pun {float f; uint32_t u;};

void encodeFloat(uint16_t *regs, float x)
{
    union Pun pun;

    pun.f = x;
    regs[0] = (pun.u >> 16) & 0xFFFFU;
    regs[1] = pun.u & 0xFFFFU;
}
*/
