/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>
#include "DHT.h"
//#include <Wire.h>
//#include <ACROBOTIC_SSD1306.h>
#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);
//    Wire.begin();
//    oled.init();
//    oled.clearDisplay();
//    oled.setTextXY(0,0);
//    oled.putString("Welcome");
    delay(10);
    wifiSetup();
    dht.begin();
}


void loop() {
    const uint16_t port = 8001;
    const char * host = "192.168.1.11"; // ip or dns
    
    
    
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }

    delay(2000);
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    
    if(isEmpty(h,t,f)) {
      Serial.println("Failed to read from DHT sensor!"); //Send error to monitor
      client.println("data not found"); //Send error to server
      String line = client.readStringUntil('\r');
      client.println(line);
      Serial.println("closing connection");
      client.stop();
      Serial.println("wait 5 sec...");
      delay(5000);
      return;
    } else {
      sendingData(h,t,f,client);
    }
    
    //read back one line from server
    String line2 = client.readStringUntil('\r');
    client.println(line2);

    Serial.println("closing connection");
    client.stop();
    
    Serial.println("wait 5 sec...");
    delay(5000);
}

void wifiSetup() {
    WiFiMulti.addAP("Room3-2", "room5125");
    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");
    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

boolean isEmpty(float h,float t, float f) {
  if (isnan(h) || isnan(t) || isnan(f)) {
      return true;
    }
  return false; 
}

void sendingData(float h,float t,float f, WiFiClient client) {
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    // This will send the request to the server
    client.print("Humidity: ");
    client.print(h);
    client.print("% ");
    client.print("Temperature: ");
    client.print(t);
    client.print(" *C ");
    client.print(f);
    client.print(" *F ");
    client.print("Heat index: ");
    client.print(hic);
    client.print(" *C ");
    client.print(hif);
    client.println(" *F");
 }
