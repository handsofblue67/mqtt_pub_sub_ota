#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <MQTT.h>

#define CLIENT_ID "esp3" //increment this with every new board
#define TOPIC_SUB0 "/espLights/all/off"
#define TOPIC_SUB1 "/espLights/all/on"
#define TOPIC_PUB "/espLights/status/esp3" //increment this with every new board

// create MQTT
MQTT myMqtt(CLIENT_ID, "192.168.1.3", 1883);

const char* host = "esp1-ota";
const char* ssid = "your ssid";
const char* pass = "ssid password";
const uint16_t aport = 8266;

WiFiServer TelnetServer(aport);
WiFiClient Telnet;
WiFiUDP OTA;

boolean bIsConnected = false;
int relayState;

void setup() { 
  pinMode(12, OUTPUT);
  relayState = 0;
  digitalWrite(12, relayState);
  
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Arduino OTA Test");

  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());
    
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to MQTT server");  

  // setup callbacks
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  
  Serial.println("connect mqtt...");
  myMqtt.connect();

  delay(10);
  
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    MDNS.addService("arduino", "tcp", aport);
    OTA.begin(aport);
    TelnetServer.begin();
    TelnetServer.setNoDelay(true);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  //OTA Sketch
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if(Update.end()){
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
  //IDE Monitor (connected to Serial)
  if (TelnetServer.hasClient()){
    if (!Telnet || !Telnet.connected()){
      if(Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
    } else {
      WiFiClient toKill = TelnetServer.available();
      toKill.stop();
    }
  }
  if (Telnet && Telnet.connected() && Telnet.available()){
    while(Telnet.available())
      Serial.write(Telnet.read());
  }
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t * sbuf = (uint8_t *)malloc(len);
    Serial.readBytes(sbuf, len);
    if (Telnet && Telnet.connected()){
      Telnet.write((uint8_t *)sbuf, len);
      yield();
    }
    free(sbuf);
  }
  delay(1);
   if (!bIsConnected) {
    // try to connect to mqtt server
    myMqtt.connect();
  }
  delay(1);
}

void myConnectedCb() {
  Serial.println("connected to MQTT server");
  bIsConnected = true;

  Serial.println("subscribe to topic...");
  myMqtt.subscribe(TOPIC_SUB0);
  myMqtt.subscribe(TOPIC_SUB1);
}

void myDisconnectedCb() {
  Serial.println("disconnected. try to reconnect...");
  bIsConnected = false;
}

void myPublishedCb() {
  Serial.println("published status: " + relayState);
}
/*---------------------Switch relay on---------------------*/
void myDataCb(String& topic, String& data) {
  String state;
  if (topic == TOPIC_SUB1) {
    if (relayState == 0) {
    relayState = 1;
    digitalWrite(12, relayState);
    state = "State changed to ";
    state += relayState;
  } else {
    digitalWrite(12, relayState);
    state = "State was already ";
    state += relayState;
    }

  }
  
  /*---------------------Switch relay off---------------------*/
  else if (topic == TOPIC_SUB0) {
    if (relayState == 1) {
    relayState = 0;
    digitalWrite(12, relayState);
    state = "State changed to ";
    state += relayState;

  } else {
    digitalWrite(12, relayState);
    state = "State was already ";
    state += relayState;
    }
  } else {
    Serial.print("topical error: ");
  }
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(data);
  boolean result = myMqtt.publish(TOPIC_PUB, state);
}
