
#include <painlessMesh.h>

// some gpio pin that is connected to an BUZEER...

#define  BUZZER             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2
#define  WAIT_BUTTON        4
#define  CALL_BUTTON        5

#define   BUZZ_PERIOD    3000 // milliseconds until cycle repeat
#define   BUZZ_GAP    10000 
#define   BUZZ_ITERATIONS   5  //no.of buzzes per one gap
#define   MESH_SSID       "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define   GUARD_NODE      2490350279
bool isBusThere=false;
String busThereCheck="Bus is there";
String busNotThereCheck="Bus is not there";
bool isTaskBuzzEnable=false;
bool onFlag=false;
int iteration_count=1;
int isCall=0;
int isWait=0;

// Prototypes
void buzz();
bool callBus();
bool waitBus();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

//Task taskBuzz(BUZZ_PERIOD, TASK_FOREVER, &buzz);

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(WAIT_BUTTON, INPUT);
  pinMode(CALL_BUTTON, INPUT);
  mesh.setDebugMsgTypes(ERROR | DEBUG);
  mesh.setContainsRoot(true);
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  
//  userScheduler.addTask( taskBuzz );

//  randomSeed(analogRead(A0));

}

void loop() {
  mesh.update();
//  if(isBusThere && !isTaskBuzzEnable)
//  {
//    taskBuzz.enable();
//    isTaskBuzzEnable=true;
//  }
//  else if(!isBusThere && isTaskBuzzEnable)
//  {
//    taskBuzz.disable();
//    isTaskBuzzEnable=false;
//  }
  digitalWrite(BUZZER,onFlag);
  isCall=digitalRead(CALL_BUTTON);
  if(isCall && !isBusThere)
      if(!callBus())
          Serial.println("ERROR: callBus Failed to send message!!");
  isWait=digitalRead(WAIT_BUTTON);
  if(isWait && isBusThere)  
      if(!waitBus())
          Serial.println("ERROR:waitBus Failed to send message!!");
}



//void buzz(){
//  if(iteration_count==2*BUZZ_ITERATIONS)
//  {
//    taskBuzz.setInterval(BUZZ_GAP);
//    iteration_count=0;
//  }
//  else if(iteration_count==1)
//  {
//    taskBuzz.setInterval(BUZZ_PERIOD);
//  }
//  iteration_count+=1;
//  if(onFlag)
//    onFlag=false;
//  else
//    onFlag=true;
//  
//}

bool callBus()
{
  String msg="call the bus";
  return mesh.sendSingle(GUARD_NODE, msg);
}
bool waitBus()
{
  String msg="wait";
  return mesh.sendSingle(GUARD_NODE, msg);
}


void receivedCallback(uint32_t from, String & msg) {
//  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(from==GUARD_NODE)
  {
    if(busThereCheck == msg.c_str())
    {
      isBusThere=true;
    }
    else if(busNotThereCheck == msg.c_str())
    {
      isBusThere=false;
    }
  }
}

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
