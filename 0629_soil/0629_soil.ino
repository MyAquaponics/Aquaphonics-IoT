#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define SensorPin A0            
#define Offset 0.00
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40
int pHArray[ArrayLenth];   
int pHArrayIndex=0;  

static float data;

const char* ssid = "iptime_micro";
const char* password = "visioninside";
const char* mqtt_server = "192.168.0.4";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup_wifi(void){

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
 } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  pinMode(SensorPin, INPUT);
  client.setServer(mqtt_server, 1883);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // put your main code here, to run repeatedly:
  data=analogRead(SensorPin);
    snprintf(msg, 75, "%.1f", data);
  client.publish("soil",msg);
  Serial.println(msg);
  delay(2000);
}
