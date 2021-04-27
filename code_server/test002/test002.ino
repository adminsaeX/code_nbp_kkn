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
String deviceID1 = "1234561" ;// 8 digit
String deviceID2 = "1234562" ;// 8 digit
String deviceID3 = "1234563" ;// 8 digit
String deviceID4 = "1234564" ;// 8 digit
String deviceID5 = "1234565" ;// 8 digit
String deviceID6 = "1234566" ;// 8 digit
String deviceID7 = "1234567" ;// 8 digit
String deviceID8 = "1234568" ;// 8 digit
String deviceID9 = "1234569" ;// 8 digit
String deviceID10 = "12345610" ;// 8 digit
String projectID = "AYA1" ;// 4 digit
String api_key = "12345678";

#define DEBUG_Board  false
//=========================Define Variable system =========//
const long interval_send = 10000 ; // time to send data genaral millisecond
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
int DCVolt = 3000; // PV Volt
int DCAmp = 100; // PV Current
int DCAmp2 = 0;
int DCWatt = 3000; // PV Power
float DCWh; // PV Energy
float DCkWh ;// PV Energy kWh
int ACVolt = 0; // AC Volt
int ACAmp = 0; // AC Current
int ACWatt = 0; // AC Power
float ACWh; // AC Energy
float FlowRate = 0; // Flowrate
float QTotal; //Water total
int Temp = 0; //Ambient Temperature
int Hum = 0; //Humidity
byte Fault = 0; //Fault
byte PumpState = 0; //Pump State
int PumpHz = 0; // Pump Hz
byte DigitalIn =0 ; //
int OutVolt =0 ; // Output Voltage
int OutAmp =0; // output Current
int OutPower =0; // output power
int RPM = 0; // output rpm
int solarEnergy = 0;
int PVTemp = 0 ;
unsigned long checkSum=0;
char crc[16];
int FlowScal = 1;
float FlowRateM = 0;
float FlowRateQ = 0;
int sumFlow = 0;
int sumEnergy =0;
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
uint16_t au16data[32];
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

modbus_t telegram[32];

unsigned long u32wait;
/////////////inverter BPD invt////////////////////////
byte ID_inverter = 1 ;
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
int energy_total = 0; // byte 0 - 7
int water_total = 8 ; // byte 8 - 15


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
  attachInterrupt(FlowPin1,pulseCount,FALLING); // flow count
  QTotal = readEEPROM(water_total);

  DCWh =   readEEPROM(energy_total);
  TempBoard.begin();
  master.begin(9600);
  master.setTimeOut(2000);
  u32wait = millis() + 1000;
  u8state = 0;
  u8query = 0;

 
  
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
  if (u8query > 11) u8query = 0;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 200;
      digitalWrite(led_g,HIGH);
      delay(200);
      digitalWrite(led_g,LOW);
      delay(100); 
    }
    break;
  }
      
 // DCVolt    = au16data[0];
  
 // DCAmp     = au16data[1];
 // DCAmp2    = au16data[12];
 // DCWatt    = au16data[2];
  //PumpHz    = au16data[3];
 // RPM       = au16data[4];
 // PumpState = au16data[5];
 // Fault     = 0;
  //OutVolt   = au16data[7];
  //OutAmp    = au16data[8];
  //OutPower  = au16data[9];
  //Temp      = au16data[10];
  //Hum       = au16data[11];

  
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
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
String message02 =  "{\"api_key\":\""+api_key+"\","
                    +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID2+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message03 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID3+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message04 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID4+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message05 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID5+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message06 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID6+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message07 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID7+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message08 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID8+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message09 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID9+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
                   
String message10 =  "{\"api_key\":\""+api_key+"\","
                   +"\"pro_ID\":\""+projectID+"\","
                   +"\"dev_ID\":\""+deviceID10+"\","
                   +"\"data\":[{\"dcV\":\""+DCVolt+"\","
                   +"\"dcA\":\""+DCAmp+"\","
                   +"\"dcW\":\""+DCWatt+"\","
                   +"\"dcWh\":\""+DCWh+"\","
                   +"\"fault\":\""+Fault+"\"}]}";
           
            
                 
 

 // Serial.println(message);
 // delay(2000);


//String sTr = "{\"data\":[{\"dcV\":\"200\",\"dcA\":\"10\"}]}";


 //========================Send Data General ============================//
 
    currentMillis = millis();
 if(currentMillis - previousMillis >= interval_send)
 {

  String udpHex1 = AISnb.str2HexStr(message01);
  
  UDPSend udp1 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex1);
  delay(1000);
  String udpHex2 = AISnb.str2HexStr(message02);
  UDPSend udp2 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex2);
  delay(1000);
  String udpHex3 = AISnb.str2HexStr(message03);
  UDPSend udp3 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex3);
  delay(1000);
  String udpHex4 = AISnb.str2HexStr(message04);
  UDPSend udp4 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex4);
  delay(1000);
  String udpHex5 = AISnb.str2HexStr(message05);
  UDPSend udp5 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex5);
  delay(1000);
  String udpHex6 = AISnb.str2HexStr(message06);
  UDPSend udp6 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex6);
  delay(1000);
  String udpHex7 = AISnb.str2HexStr(message07);
  UDPSend udp7 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex7);
  delay(1000);
  String udpHex8 = AISnb.str2HexStr(message08);
  UDPSend udp8 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex8);
  delay(1000);
  String udpHex9 = AISnb.str2HexStr(message09);
  UDPSend udp9 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex9);
  delay(1000);
  String udpHex10 = AISnb.str2HexStr(message10);
  UDPSend udp10 = AISnb.sendUDPmsg(serverIP,serverPort,udpHex10);
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
 calFlow();
 calPower();
 //wdt_reset();
}

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
 void calPower()
 {
   currentCal_power = millis();
   if(currentCal_power - preCal_power >=interval_power)
   {
    DCWh += (DCWatt*(interval_power/1000))/36000.0; ; //Wh
    
    DCkWh = DCWh/1000.0; // kWh
    if(DCWh > 3.4E+38)
    {
       for (int i = 0 ; i < 4 ; i++) {
    EEPROM.write(energy_total+i, 0);
     }
     DCWh =   readEEPROM(energy_total);
    }
    if(DEBUG_Board)
    {
      Serial.print("DcWh=");
      Serial.print(DCWh);
      Serial.print(" DcWatt=");
      Serial.println(DCWatt);
    }
    preCal_power = currentCal_power;
    
    sumEnergy++;
    
    if(sumEnergy >=20)
    {
      writeEEPROM(energy_total,DCWh);
      sumEnergy=0;
    }
    
    
   }
   
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
