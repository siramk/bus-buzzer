

#include <painlessMesh.h>
#include <ESP8266HTTPClient.h>

#define BUS_NODE   1000
#define ROOM_NODE   2000



#define MESH_SSID   "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT     5555



#define   STATION_SSID     "IITBhilai"
#define   STATION_PASSWORD ""
#define   STATION_PORT     80
uint8_t   station_ip[4] =  {10,3,62,70}; // IP of the server

const char* ssid = "YOUR WIFI NETWORK NAME";
const char* password = "YOUR WIFI PASSWARD";

void checkDB();
bool tellBusToGo();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

bool isBusThere=false;
bool isCalled=false;

String url_1="domainname.com?requesttype=1&destination=ena";
String url_2="domainName.com?requesttype=2&destination=ena";

HTTPClient http_1;
HTTPClient http_2;


Scheduler userScheduler;
painlessMesh mesh;
SimpleList<uint32_t> nodes;

Task taskCheckDB(TASK_SECOND*1,TASK_FOREVER,&checkDB);

void setup() {
   Serial.begin(115200);
    
    
    mesh.setDebugMsgTypes(ERROR | DEBUG);
     mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT,WIFI_AP_STA, 6);
    mesh.initOTA("bridge");
    mesh.stationManual(STATION_SSID, STATION_PASSWORD, STATION_PORT, station_ip);

    mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);
    
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);

    userScheduler.addTask(taskCheckDB);
    taskCheckDB.enable();
}

void loop() {
  mesh.update();
  if(isBusThere && isCalled)
  {
    if(!tellBusToGo())
    {
      Serial.println("ERROR: tellBusToGo Failed!!");  
    }
  }
  
}
bool tellBusToGo()
{
  String msg="go to destination";
  return mesh.sendSingle(BUS_NODE,msg);
}
void checkDB()
{  http_1.begin(url_1);
  int http_1_code=http_1.GET();
  String payload;
  if(http_1_code>0)
  {
    if(http_1_code==HTTP_CODE_OK)
    {
      payload=http_1.getString();
    }
  }
  else
  {
    Serial.println("[HTTP] GET.... Failed");  
  }
  if(payload=="1")
  {
    isCalled=true;
  }
  http_1.end();
}
void receivedCallback(uint32_t from, String & msg) {

    if( (from==BUS_NODE or from==ROOM_NODE) && msg.c_str()=="going to destination")
    {  http_2.begin(url_2);
      isCalled=false;
      isBusThere=false;
      int http_2_code=http_2.GET();
      if(http_2_code>0)
      { 
        if(http_2_code ==HTTP_CODE_OK)
        {
          String payload=http_2.getString();
        }
      }
      else
      {
        Serial.println("[HTTP] GET.... Failed");
      }
      http_2.end();
     }
}

void newConnectionCallback(uint32_t nodeId)
{
  if(nodeId==BUS_NODE)
  {
    isBusThere=true;
  }
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
 
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
