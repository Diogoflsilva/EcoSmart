// Libraries
#include <FS.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>


//-------- Sensor 1 --------
const int T1echoPin = 26;
const int T1trigPin = 25;

int T1duration = 0;
int T1distance = 0;
//--------------------------

//-------- Sensor 2 --------
const int T2echoPin = 33;
const int T2trigPin = 32;

int T2duration = 0;
int T2distance = 0;
//--------------------------

//-------- Sensor 3 --------
const int T3echoPin = 19;
const int T3trigPin = 21;

int T3duration = 0;
int T3distance = 0;
//--------------------------

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *T1topic = "AI4F/T1Status";
const char *T2topic = "AI4F/T2Status";
const char *T3topic = "AI4F/T3Status";
const char *mqtt_username = "AI4F_Username";
const char *mqtt_password = "";
const int mqtt_port = 1883;
char receivedChar;


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  // Setting the Pins to INPUT or OUTPUT
  pinMode(T1trigPin, OUTPUT);
  pinMode(T1echoPin, INPUT);

  pinMode(T2trigPin, OUTPUT);
  pinMode(T2echoPin, INPUT);

  pinMode(T3trigPin , OUTPUT);
  pinMode(T3echoPin , INPUT);

  // Initializing the Serials
  Serial.begin(115200);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("ESPE WiFi Manager", "admin123")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //connecting to the MQTT Broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id;
    client_id += String(WiFi.localIP());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected");
    }
  }
  // Subscribing to the topics
  client.subscribe(T1topic);
  client.subscribe(T2topic);
  client.subscribe(T3topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '1') {
      //Put Something here if you want something to happen when recieve 1
    }
  }
}

void loop() {
  //---------- Sensor 1 -----------
  digitalWrite(T1trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(T1trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(T1trigPin, LOW);
  T1duration = pulseIn(T1echoPin, HIGH);
  T1distance = T1duration / 58.2;

  Serial.print("T1 Distance: ");
  Serial.print(T1distance);
  Serial.println("cm");

  String t1d = String(T1distance);

  client.publish(T1topic, (char*) t1d.c_str());
  client.subscribe(T1topic);
  //-------------------------------

  //---------- Sensor 2 -----------
  digitalWrite(T2trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(T2trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(T2trigPin, LOW);
  T2duration = pulseIn(T2echoPin, HIGH);
  T2distance = T2duration / 58.2;

  Serial.print("T2 Distance: ");
  Serial.print(T2distance);
  Serial.println("cm");

  String t2d = String(T2distance);

  client.publish(T2topic, (char*) t2d.c_str());
  client.subscribe(T2topic);
  //-------------------------------

  //---------- Sensor 3 -----------
  digitalWrite(T3trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(T3trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(T3trigPin, LOW);
  T3duration = pulseIn(T3echoPin, HIGH);
  T3distance = T3duration / 58.2;

  Serial.print("T3 Distance: ");
  Serial.print(T3distance);
  Serial.println("cm");

  String t3d = String(T3distance);

  client.publish(T3topic, (char*) t3d.c_str());
  client.subscribe(T3topic);

  //-------------------------------

  delay(1000);
  client.loop();
}
