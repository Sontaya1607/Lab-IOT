#include <ESP8266WiFi.h>
#include <MicroGear.h>
const char* ssid= "son_tyty";
const char* password = "golf160797";

#define APPID   "SontytyHome"
#define KEY     "eirFfq673g8oltC"
#define SECRET  "r7HhOvF0v3GukVooGq4XQ0cTo"
#define ALIAS   "Board1"
#define TARGET   "SontytyRoom"

#define LED1     D0
#define LED2     D4

WiFiClient client;

int timer = 0;
MicroGear microgear(client);
void onMsghandler(char* topic, uint8_t* msg, unsigned int msglen){
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for(int i= 0; i< msglen; i++){
    strState[i]=(char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();
  String stateStr = String(strState).substring(0, msglen);
  
  if(stateStr == "CH1ON")
  {
    digitalWrite(LED1,HIGH); 
    Serial.println("ON LED 1");
  }
  else if(stateStr == "CH1OFF")
  {
    digitalWrite(LED1,LOW);  
    Serial.println("OFF LED 1");
  }

  if(stateStr == "CH2ON")
  {
    digitalWrite(LED2,HIGH); 
    Serial.println("ON LED 2");
  }
  else if(stateStr == "CH2OFF")
  {
    digitalWrite(LED2,LOW);  
    Serial.println("OFF LED 2");
  }

  if(stateStr == "CH3ON")
  {
    digitalWrite(LED1,HIGH); 
    digitalWrite(LED2,HIGH); 
    Serial.println("ON LED 1 & 2");
  }
  else if(stateStr == "CH3OFF")
  {
    digitalWrite(LED1,LOW); 
    digitalWrite(LED2,LOW);  
    Serial.println("OFF LED 1 & 2");
  }
}
void onFoundgear(char* attribute, uint8_t* msg, unsigned int msglen){
  Serial.print("Found new member --> ");
  for(int i=0; i<msglen; i++)
  Serial.print((char)msg[i]);
  Serial.println();
}
void onLostgear(char* attribute, uint8_t* msg, unsigned int msglen){
  Serial.print("Lost member --> ");
  for(int i=0; i<msglen; i++)
  Serial.print((char)msg[i]);
  Serial.println();
}
/*When a microgear is connected, dothis*/
void onConnected(char* attribute, uint8_t* msg, unsigned int msglen){
  Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgearALIAS */
  microgear.setAlias(ALIAS);
}
void setup()
{
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(PRESENT,onFoundgear);
  microgear.on(ABSENT,onLostgear);
  microgear.on(CONNECTED,onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);

  if(WiFi.begin(ssid, password))
  {
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("WiFiconnected");
  microgear.init(KEY,SECRET,ALIAS);
  microgear.connect(APPID);
}
void loop()
{
  if(microgear.connected()){
    microgear.loop();
    bool led1 = digitalRead(LED1);
    bool led2 = digitalRead(LED2);
    String status_LED1;
    String status_LED2;
    String status_AllLED;
    
    if (led1 < 2) {
      if(led1 == HIGH){
        status_LED1="CH1ON";
      }
      else{
        status_LED1="CH1OFF";
      }
    }

    if (led2 < 2) {
      if(led2 == HIGH) {
        status_LED2="CH2ON";
      }
      else {
        status_LED2="CH2OFF";
      }
    }

    if (led1 < 2 && led2 < 2) {
      if(led1 == HIGH && led2 == HIGH) {
        status_AllLED="CH3ON";
      }
      else {
        status_AllLED="CH3OFF";
      }
    }
    
    String data2freeboard = status_LED1 + "," + status_LED2 + "," + status_AllLED;
    microgear.chat(TARGET,data2freeboard);
    Serial.print("Send message to NetPie: ");
    Serial.println(data2freeboard);
  }
  else{
    Serial.println("connection lost, reconnect...");
    if(timer >= 5000){
      microgear.connect(APPID);
      timer=0;
    }
    else timer += 100;
  }
  delay(1000);
}
