#include <painlessMesh.h>

#define GUARD_NODE  3000
#define ACAD_NODE   4000

#define MESH_SSID "whateverYouLike"
#define MESH_PASSWORD  "somethingSneaky"
#define MESH_PORT 5555

#define GOING_BUTTON     4
#define BUZZER_PIN       5

//prototypes

void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);


painlessMesh mesh;


SimpleList<uint32_t> nodes;
int goingToDestination;

void setup() {
Serial.begin(115200);
  mesh.setDebugMsgTypes(ERROR | DEBUG);

  mesh.init(MESH_SSID,MESH_PASSWORD,MESH_PORT,WIFI_AP_STA);
  mesh.setContainsRoot(true);

    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);
    
    pinMode(GOING_BUTTON,INPUT);
    pinMode(BUZZER_PIN,OUTPUT);
  
}

void loop() {
  mesh.update();
  goingToDestination=digitalRead(GOING_BUTTON);
  if(goingToDestination)  
    if(!tellAcadNode("going to destination"))
        Serial.println("ERROR: tellAcadNode Failed!!");
   
  

}
bool tellAcadNode(String msg) //doubt: can i use this type of parameter
{
    return mesh.sendSingle(ACAD_NODE,msg);
}

void receivedCallback(uint32_t from, String & msg)
{
  if(from == GUARD_NODE && msg.c_str()=="wait")
  {
//      digitalWrite(BUZZER_PIN,HIGH);
//      doDelay(3000);
//      digitalWrite(BUZZER_PIN,LOW);
//      

        Serial.println("from guard_node: wait");
  }
  else if(from == ACAD_NODE && msg.c_str()=="go to destination")
  {
//    digitalWrite(BUZZER_PIN,HIGH);
//    doDelay(3000);
//    digitalWrite(BUZZER_PIN,LOW);
//    doDelay(5000);
//    digitalWrite(BUZZER_PIN,HIGH);
//    doDelay(3000);
//    digitalWrite(BUZZER_PIN,LOW);     
    Serial.println("from aca_node: go to destination");
  } 
}

//void doDelay(int duration)
//{
//  int current=millis();
//  int prev=millis();
//  while(current-prev<duration)
//  {
//    prev=current;
//    current=millis();
//  }
//  
//}


void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
//  onFlag = false;
//  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
//  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
//  onFlag = false;
//  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
//  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
//  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
