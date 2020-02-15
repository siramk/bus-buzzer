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
int val;
uint8_t inputpin=2;
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1,adress2);
  radio.openWritingPipe(adress1);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  state=defstate;
  val=0;
  pinMode(inputpin,INPUT);
}
void listenn()
{
  radio.startListening();
  if(radio.available())                     //if the bus is nearby
  {
      char msg[5]="0000";
      radio.read(&msg,sizeof(msg));
      if(state==1 && msg=="0001")state=2;
      if(state==2 && msg=="0010")state=3;
     
  }
  else
  {
    
  }
   radio.stopListening();
  
}
boolean isbuttonpressed()
{
  val = digitalRead(inputpin);  // read input value
  if (val == HIGH) {         // check if the input is HIGH (button released)
    return 0; // turn LED OFF
  } else {
  
    Serial.println("button pressed");
    return 1;
  }
}
void loop() {
    listenn();
    if((state==0))
    {
      if(isbuttonpressed())state=1;
    }
    if(state==1)
    {
      char msg[5]="0001";
      radio.write(&msg,sizeof(msg));
    }
    if(state==3)
    {
      char msg[5]="0010";
      radio.write(&msg,sizeof(msg));
    }
   

}
   
   
  
