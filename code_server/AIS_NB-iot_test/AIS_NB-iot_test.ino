#include "AIS_NB_BC95.h"
#include "DHT.h"
String apnName = "devkit.nb";

String serverIP = "34.73.53.90"; // Your Server IP
String serverPort = "41234"; // Your Server Port

String devname = "mega2560";
String devID = "002";
AIS_NB_BC95 AISnb;

const long interval = 5000;  //millisecond
unsigned long previousMillis = 0;

#define DHTPIN 10
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temp ;
float hum ;
void setup()
{ 
  AISnb.debug = true;
  
  Serial.begin(9600);
 
  AISnb.setupDevice(serverPort);

  String ip1 = AISnb.getDeviceIP();  
  delay(1000);
  
  pingRESP pingR = AISnb.pingIP(serverIP);
  previousMillis = millis();
  dht.begin();
}
void loop()
{ 
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval)
    {
      temp = dht.readTemperature();
      hum = dht.readHumidity();

     
      if (isnan(hum) || isnan(temp) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
      

      String message = "{\"device_ID\":\"" + String(devID) + "\",\"device_name\":\""+String(devID)+"\",\"temp\":\"" + String(temp) + "\",\"hum\":\"" +String(hum)+ "\",\"gps\":[13.8180543,100.0362107]}"; 
      Serial.println(message);  
      
      // Send data in String 
      UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, message);
   
      //Send data in HexString     
      //udpDataHEX = AISnb.str2HexStr(udpData);
      //UDPSend udp = AISnb.sendUDPmsg(serverIP, serverPort, udpDataHEX);
      previousMillis = currentMillis;
  
    }
  UDPReceive resp = AISnb.waitResponse();
  
     
}
void alarm(int t)
{
  if(t>50)
  {
    
  }
  
}
