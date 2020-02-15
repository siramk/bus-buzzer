#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <ESP8266HTTPClient.h>

//wifi connected code
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

RF24 radio(7, 8); // CE, CSN
const byte adress1[6] = "00001";
const byte adress2[6]="11110";
const String ssid="abcd";
const String password="abcd1234";
int state=0;
const String dest="ena";
const String url2="http://desolate-beach-70001.herokuapp.com/bus.php?requesttype=1&destination=ena";
const String url1="http://desolate-beach-70001.herokuapp.com/guard.php?destination=ena";
const int defstate=0;
void setup() {
  Serial.begin(9600);
  //radio.begin();
  //radio.openReadingPipe(1,adress1);
  //radio.openWritingPipe(adress2);
 // radio.setPALevel(RF24_PA_MIN);
 // radio.startListening();
  state=defstate;   
  WiFi.begin(ssid, password);
  
}
String confirmation(String url)
{
  return callbus(url);
}
String callbus(String url)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting");
  }
  else
  {
    Serial.println("Connected");
    HTTPClient http; 
    http.begin(url);
    int httpCode = http.GET();
    Serial.print("get code");    
    Serial.println(httpCode);                                    
    if (httpCode > 0) 
    { 
      String payload = http.getString(); 
      Serial.println(payload);
      
          return payload;
    }
}
return "-1";
}

void loop() {
  //srand(time(NULL));
  
 // char currentstate[4]="0000";
  Serial.print("state=");
  Serial.println(state);
   radio.startListening();
  if(radio.available())                     
  {
   
   
    char msg[5]="0000";
    radio.read(&msg,sizeof(msg));
    if(state==0 && msg=="0001")
    {
      state=1;
    }
    else if(state==3 && msg=="0010")
    {
      state=0;
    }
    
  }
   radio.stopListening();
   if(state==1)
   {
     if(callbus(url1)=="1")state=2;;
   }
   else if(state==2)
   {
     if(confirmation(url2)=="0")state=3;;
     char msg[5]="0001";
     radio.write(&msg,sizeof(msg));
     
   }
  
   
   

}
