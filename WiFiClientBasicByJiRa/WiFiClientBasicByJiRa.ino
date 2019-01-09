/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>
#include "DHT.h"
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>
#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    oled.init();
    oled.clearDisplay();
    oled.setTextXY(0,0);
    oled.putString("Welcome");
    delay(10);
    wifiSetup();
    dht.begin();
}


void loop() {
    const uint16_t port = 8001;
    const char * host = "172.20.10.7"; // ip or dns
    
    
    
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
      client.println("Data not found!"); //Send error to server
      String line = client.readStringUntil('\r');
      client.println(line);
      oled.clearDisplay();
      oled.setTextXY(0,0);
      oled.putString("Data not found!");
      Serial.println("closing connection");
      client.stop();
      Serial.println("wait 5 sec...");
      delay(5000);
      return;
    } else {
      Serial.println("Data is sending...");
      oledPrint(h,t,f);
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
    WiFiMulti.addAP("Joseph", "759759123");
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

 void oledPrint(float h,float t,float f) {
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    oled.clearDisplay();
    oled.setTextXY(0,0);
    oled.putString(" H: ");
    oled.putString(String(h));
    oled.putString("% ");
    oled.setTextXY(1,0);
    oled.putString(" T: ");
    oled.putString(String(t));
    oled.putString(" *C ");
    oled.setTextXY(1,0);
    oled.putString(String(f));
    oled.putString(" *F ");
    oled.setTextXY(1,0);
    oled.putString(" HI: ");
    oled.putString(String(hic));
    oled.putString(" *C ");
    oled.putString(String(hif));
    oled.putString(" *F");
    oled.setTextXY(1,0);
  }
