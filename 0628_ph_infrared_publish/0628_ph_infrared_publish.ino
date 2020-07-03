#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define SensorPin A0
#define TCRT5000 4 // TCRT5000 센서를 아두이노 A1에 연결            
#define Offset 0.00
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40
int pHArray[ArrayLenth];   
int pHArrayIndex=0;  
int count=0;

const char* ssid = "iptime_micro";
const char* password = "visioninside";
const char* mqtt_server = "192.168.0.4";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char msg_msg[50];

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

void setup(void){
  Serial.begin(115200);
  setup_wifi();
  pinMode(TCRT5000, INPUT);
  client.setServer(mqtt_server, 1883);
}

void loop(void){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 센서 값을 읽어들여 적외선 값 저장
  int value = digitalRead(TCRT5000); // 센서를 읽어서 value에 저장

  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;

  
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset-5;
      samplingTime=millis();
  }

  if(millis() - printTime > printInterval)
//Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
    Serial.print(voltage,2);
    Serial.print("    pH value: ");
    Serial.println(pHValue,2);
    snprintf(msg, 75, "%.3f", pHValue);
    client.publish("ph",msg);
    Serial.println(msg);
    Serial.print("infrared value: "+value);
 
 if(value == 0) {
  count = count+1;
   sprintf(msg_msg,"%d", count);
 client.publish("infrared",msg_msg);
  Serial.println(msg_msg); 
 
    printTime=millis();
    
  } 
  delay(2000); 
 
 }
 

}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0]; max=arr[1];
    }
    else{
      min=arr[1]; max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
