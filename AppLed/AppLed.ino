#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char* ssid = "$-God";
const char* password = "yubk2033";

#define APPID   "SontytyHome"
#define KEY     "eirFfq673g8oltC"
#define SECRET  "r7HhOvF0v3GukVooGq4XQ0cTo"
#define ALIAS   "esp8266"

#define LED_PIN D1

WiFiClient client;
MicroGear microgear(client);

long timer = 0;
bool reconnect = false;

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    String iMsg = "";
    for (int i=0;i<msglen;i++) iMsg += (char)msg[i];
    
    Serial.println(topic);
    Serial.println(iMsg);

    if (iMsg == "GET") {
      microgear.chat("/led", digitalRead(LED_PIN) == HIGH ? "ON" : "HIGH");
    } else if (iMsg == "ON") {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setName("led");
}


void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  /* Event listener */
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);
  
  microgear.init(KEY,SECRET,ALIAS);

}

void loop() {
  if (microgear.connected()) microgear.loop();
    
  if (microgear.connected() && !reconnect) {
    reconnect = true;
    Serial.println("\nConnected");
  } else if (!microgear.connected() && reconnect) {
   if ((millis()-timer)%500 == 0) Serial.print(".");
    if ((millis()-timer)>5000) reconnect = false;
  } else if (!microgear.connected() && !reconnect) {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
    timer = millis();
  }
}
