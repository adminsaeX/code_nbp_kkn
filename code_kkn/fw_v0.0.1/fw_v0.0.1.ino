#include <FS.h> 
//==========================Library==========================//
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 
#include <ModbusRtu.h>
#include <SoftwareSerial.h>
//=========================systems config===================//
WiFiServer server(88);

const char* apssid = "sae_gateway";
const char* appassw ="adminsae079";

IPAddress local_IP (192,168,1,2);
IPAddress gateway (192,168,1,1);
IPAddress subnet (255,255,255,0);



//=========================Program config==================//
//--------wifi--------//
char ssid[100];
char passw[100];
//--------modbus---------------//
char Device_ID[10];
char buadRate[20];
char TimeOut[20];   //ms
char Timequery[20]; //ms

//---------device-------------//
char SN[50];
char proID[50];
String ProID;
String sn;
char TimeSend[50];
char serverIP[50];
char serverPort[50];
String ServerIP;
String ServerPort;
//=====================Vaiable===========================//
String command;
char cc_command[256];
String c_ssid;
String c_passw;
String c_Device_ID;
String c_buadRate;
String c_TimeOut;
String c_Timequery;
String c_SN;
String c_proID;
String c_TimeSend;
String c_serverIP;
String c_serverPort;

char cc_ssid[100];
char cc_passw[100];
char cc_Device_ID[10];
char cc_buadRate[20];
char cc_TimeOut[20];
char cc_Timequery[20];
char cc_SN[50];
char cc_proID[50];
char cc_TimeSend[50];
char cc_serverIP[50];
char cc_serverPort[50];

int device_id;
int buadrate;
int timeout;
int timequery;
int timesend;
int di_0 = command.indexOf('&');
int di_1 = command.indexOf('&',di_0+1);
int di_2 = command.indexOf('&',di_1+1);
int di_3 = command.indexOf('&',di_2+1);
int di_4 = command.indexOf('&',di_3+1);
int di_5 = command.indexOf('&',di_4+1);
int di_6 = command.indexOf('&',di_5+1);
int di_7 = command.indexOf('&',di_6+1);
int di_8 = command.indexOf('&',di_7+1);
int di_9 = command.indexOf('&',di_7+1);
int di_10 = command.indexOf('&',di_9+1);
int di_11 = command.indexOf('&',di_10+1);
int di_12 = command.indexOf('&',di_11+1);

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
int errM ;
//==============================MODBUS===================//
uint16_t au16data[16];
uint8_t u8state;
uint8_t u8query;
unsigned long u32wait;
Modbus master(0,4,0);
modbus_t telegram[20];
SoftwareSerial modbusPort(14,12,false,256);
//=======================================================//
bool savedConfig = false;
bool notConnect = false;
unsigned long ts =0 ;
unsigned long last =0 ;

void setup(){
Serial.begin(115200);
Serial.println();
Serial.println("-------------start----------");

Serial.print("AP Config.........");
Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready !!" : "Failed !");

Serial.print("AP Setting.........");
Serial.println(WiFi.softAP(apssid,appassw) ? "Ready !!" : "Failed !");


Serial.print("AP IP Address : ");
Serial.print(WiFi.softAPIP());
Serial.println();
Serial.println("----------Read Program config----------");
if(SPIFFS.begin())
{

  Serial.println("open config");
  if(SPIFFS.exists("/config.json")){
    Serial.println("read config file");
    File configFile = SPIFFS.open("/config.json","r");
    if(configFile){
      Serial.println("opened config file");
      size_t size = configFile.size();
      std::unique_ptr<char[]>buf(new char[size]);
      configFile.readBytes(buf.get(),size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json =jsonBuffer.parseObject(buf.get());
      json.printTo(Serial);
      if(json.success()){
        Serial.println("\nparsed json");
        strcpy(ssid,json["ssid"]);
        strcpy(passw,json["passw"]);
        strcpy(Device_ID,json["Device_ID"]);
        strcpy(buadRate,json["buadRate"]);
        strcpy(TimeOut,json["TimeOut"]);
        strcpy(Timequery,json["Timequery"]);
        strcpy(SN,json["SN"]);
        strcpy(proID,json["proID"]);
        strcpy(TimeSend,json["TimeSend"]);
        strcpy(serverIP,json["serverIP"]);
        strcpy(serverPort,json["serverPort"]);
        
      }
      else{
        Serial.println("failed to load config file");
      }
      configFile.close();
      Serial.println("read config done");
      
    }
  }
}


 

server.begin();
 
 
  
//---------------Connect Wifi-----------------//
Serial.println("Connecting Wifi");

  WiFi.begin(ssid,passw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println("Setting MODBUS");
  
  sscanf(Device_ID, "%d", &device_id);
  sscanf(buadRate, "%d", &buadrate);
  sscanf(TimeOut, "%d", &timeout);
  sscanf(Timequery, "%d", &timequery);
  sscanf(TimeSend,"%d",&timesend);
  
  
  master.begin(&modbusPort,buadrate);
  master.setTimeOut(timeout);
  u32wait = millis()+1000;
  u8state = 0 ;
  u8query = 0 ;

   // Status
  telegram[0].u8id = device_id; 
  telegram[0].u8fct = 3; 
  telegram[0].u16RegAdd = 50; 
  telegram[0].u16CoilsNo = 1; 
  telegram[0].au16reg = au16data; 

  // Voltage bus 
  telegram[1].u8id = device_id; 
  telegram[1].u8fct = 3; 
  telegram[1].u16RegAdd = 153; 
  telegram[1].u16CoilsNo = 1; 
  telegram[1].au16reg = au16data+1; 

  // Current
  telegram[2].u8id = device_id; 
  telegram[2].u8fct = 3; 
  telegram[2].u16RegAdd = 154; 
  telegram[2].u16CoilsNo = 1; 
  telegram[2].au16reg = au16data+2; 

  // Power
  telegram[3].u8id = device_id; 
  telegram[3].u8fct = 3;
  telegram[3].u16RegAdd = 156; 
  telegram[3].u16CoilsNo = 1; 
  telegram[3].au16reg = au16data+3; 

  // Temperature
  telegram[4].u8id = device_id; 
  telegram[4].u8fct = 3; 
  telegram[4].u16RegAdd = 157; 
  telegram[4].u16CoilsNo = 1; 
  telegram[4].au16reg = au16data+4; 
  // Status 
  telegram[5].u8id = device_id; 
  telegram[5].u8fct = 3; 
  telegram[5].u16RegAdd = 160; 
  telegram[5].u16CoilsNo = 1; 
  telegram[5].au16reg = au16data+5; 

  // Alarm
  telegram[6].u8id = device_id; 
  telegram[6].u8fct = 3; 
  telegram[6].u16RegAdd = 161; 
  telegram[6].u16CoilsNo = 1; 
  telegram[6].au16reg = au16data+6; 
  // digital 
  telegram[7].u8id = device_id; 
  telegram[7].u8fct = 3; 
  telegram[7].u16RegAdd = 210; 
  telegram[7].u16CoilsNo = 1; 
  telegram[7].au16reg = au16data+7; 
  // frequncy 
  telegram[8].u8id =device_id; 
  telegram[8].u8fct = 3; 
  telegram[8].u16RegAdd = 155; 
  telegram[8].u16CoilsNo = 1; 
  telegram[8].au16reg = au16data+8; 

  Serial.println("config modbus done");
  Serial.println();
  Serial.println("config server");

  size_t lenIP = strlen(serverIP);
  size_t lenPort = strlen(serverPort);
  size_t lenID = strlen(proID);
  size_t lenSN = strlen(SN);
  
  for (int i=0;i<lenIP;i++){

  ServerIP += serverIP[i];
  }
  for (int i=0;i<lenPort;i++){

  ServerPort += serverPort[i];
  }
  for (int i=0;i<lenID;i++){

  ProID += proID[i];
  }
  for (int i=0;i<lenSN;i++){

  sn += SN[i];
  }


  
  Serial.println("config server done");

  Serial.println("MODBUS RUN");
}

void loop(){
 switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    master.query( telegram[u8query] ); // send query (only once)
    u8state++;
    u8query++;
  if (u8query >10) u8query = 0;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() +timequery; 
    
    }
    break;
  }
  errM = master.getLastError();
  // parameter
  Volt = au16data[1];
  Amp  = au16data[2];
  Watt = au16data[3];
  Temp = au16data[4];
  Alarm  = au16data[6];
  Digital = au16data[7];
  Frequency = au16data[8];
  StatusRun = au16data[0];
  Status = au16data[5];
 
  
  payload = "pro_ID="+ProID
            +"&dev_ID="+sn
            +"&V="+String(Volt)
            +"&A="+String(Amp)
            +"&W="+String(Watt)
            +"&temp="+String(Temp)
            +"&alarm="+String(Alarm)
            +"&digital="+String(Digital)
            +"&freq="+String(Frequency)
            +"&statusRun="+String(StatusRun)
            +"&status="+String(Status)
            +"&lastErr="+String(errM);

   
   if(millis()-last>timesend)
  {
    sendData(payload);
    last = millis();
  }

 

 WiFiClient client = server.available();
 while(client.connected()) { 
    
    if (client.available()) { 
      
      char c = client.read(); 
      if (c == '\r') { 
        Serial.println("config connect");
        Serial.println(command); 

        
        //spilt string
        
        c_ssid = command.substring(di_0+1,di_1);
        c_passw = command.substring(di_1+1,di_2);
        c_Device_ID = command.substring(di_2+1,di_3);
        c_buadRate = command.substring(di_3+1,di_4);
        c_TimeOut = command.substring(di_4+1,di_5);
        c_Timequery = command.substring(di_5+1,di_6);
        c_SN = command.substring(di_6+1,di_7);
        c_proID = command.substring(di_7+1,di_8);
        c_TimeSend = command.substring(di_8+1,di_9);
        c_serverIP = command.substring(di_9+1,di_10);
        c_serverPort = command.substring(di_10+1,di_11);
        // convers to char array
        c_ssid.toCharArray(cc_ssid,c_ssid.length());
        c_passw.toCharArray(cc_passw,c_passw.length());
        c_Device_ID.toCharArray(cc_Device_ID,c_Device_ID.length());
        c_buadRate.toCharArray(cc_buadRate,c_buadRate.length());
        c_TimeOut.toCharArray(cc_TimeOut,c_TimeOut.length());
        c_Timequery.toCharArray(cc_Timequery,c_Timequery.length());
        c_SN.toCharArray(cc_SN,c_SN.length());
        c_proID.toCharArray(cc_proID,c_proID.length());
        c_TimeSend.toCharArray(cc_TimeSend,c_TimeSend.length());
        c_serverIP.toCharArray(cc_serverIP,c_serverIP.length());
        c_serverPort.toCharArray(cc_serverPort,c_serverPort.length());
        
        // config string
        
        strcpy(ssid,cc_ssid);
        strcpy(passw,cc_passw);
        strcpy(Device_ID,cc_Device_ID);
        strcpy(buadRate,cc_buadRate);
        strcpy(TimeOut,cc_TimeOut);
        strcpy(Timequery,cc_Timequery);
        strcpy(SN,cc_SN);
        strcpy(proID,cc_proID);
        strcpy(TimeSend,cc_TimeSend);
        strcpy(serverIP,cc_serverIP);
        strcpy(serverPort,cc_serverPort);
       


       
          
           
          Serial.println("saveing config");
          DynamicJsonBuffer jsonBuffer;
          JsonObject& json = jsonBuffer.createObject();
          json["ssid"] = ssid;
          json["passw"] = passw;
          json["Device_ID"]=Device_ID;
          json["buadRate"] = buadRate;
          json["TimeOut"] = TimeOut;
          json["Timequery"] = Timequery;
          json["SN"] = SN;
          json["proID"] = proID;
          json["TimeSend"] = TimeSend;
          json["serverIP"] = serverIP;
          json["serverPort"] = serverPort;

          File configFile = SPIFFS.open("/config.json","w");
          if(!configFile){
            Serial.println("Failed to open config file for writing");
          }
          Serial.println("configFile");
          json.printTo(Serial);
          json.printTo(configFile);
          configFile.close();
           
          
          Serial.println("save config done");
          
         
        
        command = ""; 
       
      } else if (c == '\n') { 
        Serial.println(" Disconnected config");
        
        break;
      } else { 
        command += c; 
      }
    }
 
}
}
void sendData(String payload)
{
  
  String url = "http://"+ServerIP+":"+ServerPort+"/data";
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  int httpCode = http.POST(payload);
  http.end();
 
  
}
