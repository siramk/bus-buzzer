#include <painlessMesh.h>
#include <ESP8266HTTPClient.h>

#define MESH_SSID   "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT     5555

// #define   STATION_SSID     "IITBhilai"
// #define   STATION_PASSWORD ""
// #define   STATION_PORT     80
// uint8_t   station_ip[4] =  {10,3,62,70}; // IP of the server

#define BUS_NODE   10000
#define TEST_BUTTON 14
uint32_t bus_node=BUS_NODE;

// const char* ssid = "YOUR WIFI NETWORK NAME";
// const char* password = "YOUR WIFI PASSWARD";

// Prototypes
bool informBusIsThere();
bool informBusIsNotThere();
bool callBus();
bool waitBus();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

bool isBusThere=false;
bool alreadyInformedBusIsThere=false;
bool alreadyInformedBusIsNotThere=false;
String url="";

Scheduler userScheduler;
painlessMesh mesh;
HTTPClient http;

SimpleList<uint32_t> nodes;


void setup() {
    Serial.begin(115200);
    pinMode(TEST_BUTTON,INPUT);
    mesh.setDebugMsgTypes(ERROR | DEBUG);

    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
   mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
    // mesh.initOTA("bridge");
    // mesh.stationManual(STATION_SSID, STATION_PASSWORD, STATION_PORT, station_ip);

    // mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    // mesh.setContainsRoot(true);
    
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);

}

void loop() {
  mesh.update();
  isBusThere=digitalRead(TEST_BUTTON);
  if(isBusThere)
  {
    if(!alreadyInformedBusIsThere)
    {   
        if(!informBusIsThere())
            Serial.println("ERROR: informBusIsThere Failed!!");  
        alreadyInformedBusIsThere=true;
        alreadyInformedBusIsNotThere=false;
    }
  }
  else
  {
    if(!alreadyInformedBusIsNotThere)
    {
      if(!informBusIsNotThere())
          Serial.println("ERROR: informBusIsNotThere Failed!! ");
      alreadyInformedBusIsNotThere=true;
      alreadyInformedBusIsThere=false;
    }
  }

}



bool informBusIsThere()
{
  String msg="bus is there";
  return mesh.sendBroadcast(msg);
}

bool informBusIsNotThere()
{
  String msg="bus is not there";
  return mesh.sendBroadcast(msg);
}
//TODO: to collect the id's of all wing nodes and guard nodes and bus nodes and store them, without hard coding the values

bool callBus()
{
  String msg="call the bus";
  http.begin(url);
  int httpcode=http.GET();
  String payload;
  if(httpcode > 0)
  {
    if(httpcode == HTTP_CODE_OK)
    {
        payload =http.getString();   
    }
  }
  else
  {
    Serial.println("[HTTP] GET.... Failed \n");  
  }
  http.end();

  if(payload="bus is coming")
  {
    return true;
  }
  else
  {
    return false;
  }
        
}

bool waitBus()
{
  String msg="wait";
  
  return mesh.sendSingle(bus_node,msg);
}


void receivedCallback(uint32_t from, String & msg) {
//  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(msg.c_str() == "call the bus")
  {
          if(!callBus())
          {
            Serial.println("bus is not coming!");
          }
          else
          {
            Serial.println("bus is coming!");
          }
  }
  if(msg.c_str() == "wait")
  {
      if(!waitBus())
          Serial.println("ERROR: not able to send 'wait' signal to the bus!! ");
  }
}

void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
//  onFlag = false;
//  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
//  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
  if(nodeId==BUS_NODE)
  {
    isBusThere=true;
  }
 
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
  bool isBusFlag=false;
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
    if(*node==BUS_NODE)
        isBusFlag=true;    
  }
  if(!isBusFlag)
      isBusThere=false;
  else
      isBusThere=true;
      
  Serial.println();
//  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
