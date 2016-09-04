#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <MQTT.h>
#include <Device.h>

#define CLIENT_ID "nightStand" //increment this with every new board
#define TOPIC_STATE "/nightStand/state" // subscribe to state queries
#define TOPIC_SETTINGS "/nightStand/settings" // subscribe to setting changes
#define TOPIC_ON "/espLights/all/on"
#define TOPIC_OFF "/espLights/all/off"
#define TOPIC_PUB "/espLights/status/nightStand" //increment this with every new board
#define RELAY_PIN 12
#define TOGGLE_BUTTON 14


// create MQTT
MQTT myMqtt(CLIENT_ID, "192.168.1.3", 1883);

Device device();

const char* ssid = "Stormageddon";
const char* pass = "WateryWater856";

boolean bIsConnected = false;
int relayState;

// interrupt function for physical toggle button on pin 14
void toggleInterrupt() { relayState = !relayState; }

void myConnectedCb() {
  Serial.println("connected to MQTT server");
  bIsConnected = true;
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
  if (topic == TOPIC_OFF) {
    if (relayState == HIGH) {
      relayState = LOW;
      digitalWrite(RELAY_PIN, relayState);
      state = "State changed to ";
      state += relayState;
    } else {
      digitalWrite(RELAY_PIN, relayState);
      state = "State was already ";
      state += relayState;
    }

  } else if (topic == TOPIC_ON) {
  /*---------------------Switch relay off---------------------*/
    if (relayState == LOW) {
      relayState = HIGH;
      digitalWrite(RELAY_PIN, relayState);
      state = "State changed to ";
      state += relayState;
    } else {
      digitalWrite(RELAY_PIN, relayState);
      state = "State was already ";
      state += relayState;
    }

  } else {
    Serial.print("topical error: ");
  }

  // boolean result = myMqtt.publish(TOPIC_PUB, state, 2, 1);
  boolean result = myMqtt.publish(TOPIC_PUB, state, 0, 0);
  Serial.print(topic);
  Serial.print(" ");
  Serial.println(data);
}

void setup() {

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.print(".");
  }


  /*-------------------------OTA-------------------------*/
  ArduinoOTA.setPort(8266);

  ArduinoOTA.setHostname("nightStand");

  ArduinoOTA.setPassword((const char *)"bugger_oFF");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*-------------------------MQTT---------------------------*/
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

  myMqtt.subscribe(TOPIC_ON);
  myMqtt.subscribe(TOPIC_OFF);

  delay(10);

  pinMode(RELAY_PIN, OUTPUT);

  pinMode(TOGGLE_BUTTON, INPUT);
  attachInterrupt(TOGGLE_BUTTON, toggleInterrupt, RISING);

  relayState = HIGH;

  digitalWrite(RELAY_PIN, relayState);

}

void loop() {
  if (!bIsConnected) { myMqtt.connect(); }
  delay(1);

  ArduinoOTA.handle();
}
