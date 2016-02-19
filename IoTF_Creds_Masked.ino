 /**
 * Helloworld style, connect an ESP8266 to the IBM IoT Foundation
 * 
 * Author: Ant Elder
 * License: Apache License v2
 */
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/releases/tag/v2.3

//-------- Customise these values -----------
const char* ssid = "*********";
const char* password = "**********";

#define ORG "eplnqo"
#define DEVICE_TYPE "ESP8266"
#define DEVICE_ID "thing1"
#define TOKEN "**********"

#define DHTPIN 4           //Data pin for the sensor
#define DHTTYPE DHT11      //Which version of sensor do you have? DHT11 or DHT22
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char subTopic[]= "iot-2/cmd/command/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;    //Initiate the wifiClient
PubSubClient client(server, 1883, callback, wifiClient);   //Initiate the pub-sub client

DHT dht(DHTPIN, DHTTYPE); //Initialize the DHT sensor

void setup() {
 Serial.begin(115200);
 Serial.println();

 Serial.print("Connecting to ");
 Serial.print(ssid);
 //if (strcmp (WiFi.SSID(), ssid) != 0) {
 WiFi.begin(ssid, password);
 //}
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");

 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());  
 Serial.println("");
 
 dht.begin();
  
 client.setCallback(callback);
}

int counter = 0;

void loop() {
  
  //Invoke connection function*/
  iotfConnection();
  
  //Invoke publish function*/
  iotfPublish();
 

 
}

void iotfConnection() {
  if (!!!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
     }
     Serial.println();
     iotfSubscribe();
   }
 client.loop();  
}

void iotfPublish() {
  
  delay(2000);
  float t = dht.readTemperature(true);
  Serial.println(t);
    
 String payload = "{\"d\":{\"temp\":\""+String((int)t)+"\", \"status\":\"sent from device\"}}";
 
 Serial.print("Sending payload: ");
 Serial.println(payload);
 
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
  //delay(10000); 
}

void iotfSubscribe() {
  client.subscribe(subTopic);  
}

void callback(char* subTopic, byte* payload, unsigned int length) {
 Serial.println("callback invoked");
 
  // copy the payload content into a char*
  char* data;
  data = (char*) malloc(length + 1);
  memcpy(data, payload, length);
  data[length] = '\0';
  Serial.println(String(data));
  free(data);
}


