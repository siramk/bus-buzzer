//Mesh node
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

RF24 radio(7, 8); // CE, CSN
const byte adress1[6] = "00001";
const byte adress2[6]="11110";
int defstate=0;
int state;
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1,adress2);
  radio.openWritingPipe(adress1);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  state=defstate;
}
void listenn()
{
  radio.startListening();
  if(radio.available())                     //if the bus is nearby
  {
      byte msg[4]="0000";
      radio.read(&msg,sizeof(msg);
      if(state==1 && msg=="0001")state=2;
      if(state==2 && msg=="0010")state=3;
     
  }
  else
  {
    
  }
   radio.stopListening();
  
}
void loop() {
    listenn();
    if(state==1)
    {
      byte msg[4]="0001";
      radio.write(&msg.sizeof(msg));
    }
    if(state==3)
    {
      byte msg[4]="0010";
      radio.write(&msg.sizeof(msg));
    }
   

}
   
   
   
   
   
   

}
