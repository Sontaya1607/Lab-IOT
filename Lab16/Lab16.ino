#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11
const char* ssid = "G-Son";
const char* password = "golf160797";
const char* host = "api.thingspeak.com";
const char* APIKey = "UM809KR1KEUFEI8Q";

DHT dht(DHTPIN, DHTTYPE);
void setup() {
 Serial.begin(115200);
 dht.begin();
 delay(10);
 // We start by connecting to a WiFi network
 Serial.println();
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
}
void loop() {
 delay(10000);
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
float h = dht.readHumidity();
 // Read temperature as Celsius (the default)
 float t = dht.readTemperature();
 // Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(t) ) {
  Serial.println("Failed to read from DHT sensor!");
 return;
 }
 Serial.print("connecting to ");
 Serial.println(host);

 // Use WiFiClient class to create TCP connections
 WiFiClient client;
 const int httpPort = 80;
 if (!client.connect(host, httpPort)) {
 Serial.println("connection failed");
 return;
 }
 
 // We now create a URI for the request
String url = "/update";
 url += "?key=";
 url += APIKey;
 url += "&field1=";
 url += h;
 url += "&field2=";
 url += t;

 Serial.print("Requesting URL: ");
 Serial.println(url);

 // This will send the request to the server
 client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
 unsigned long timeout = millis();
 while (client.available() == 0) {
 if (millis() - timeout > 5000) {
 Serial.println(">>> Client Timeout !");
 client.stop();
 return;
 }
 }

 // Read all the lines of the reply from server and print them to Serial
 while(client.available()){
 String line = client.readStringUntil('\r');
 Serial.print(line);
 }

 Serial.println();
 Serial.println("closing connection");
}
