#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
WiFiServer server(80); 

char   host[] = "api.thingspeak.com"; // ThingSpeak address
String APIkey = "605393";             // Thingspeak Read Key, works only if a PUBLIC viewable channel
const int httpPort = 80;
 
const char *ssid     = "Room3-2"; 
const char *password = "room5125"; 
WiFiClient client;
const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
const int ledPin = 2;
void setup(){
  Serial.begin(115200);
  pinMode(ledPin,OUTPUT);
  WiFi.begin(ssid,password);
}

void loop(){
  RetrieveTSChannelData();
  delay(1000); //Wait before we request again
}

void RetrieveTSChannelData() {  // Receive data from Thingspeak
  static char responseBuffer[3*1024]; // Buffer for received data
  client = server.available(); 
  if (!client.connect(host, httpPort)) { 
    Serial.println("connection failed"); 
    return; 
  } 
  String url = "/channels/" + APIkey; // Start building API request string
  url += "/fields/1.json?results=1";  // 5 is the results request number, so 5 are returned, 1 woudl return the last result received
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  while (!skipResponseHeaders());                      // Wait until there is some data and skip headers
  while (client.available()) {                         // Now receive the data
    String line = client.readStringUntil('\n');
    if ( line.indexOf('{',0) >= 0 ) {                  // Ignore data that is not likely to be JSON formatted, so must contain a '{'
      Serial.println(line);                            // Show the text received
      line.toCharArray(responseBuffer, line.length()); // Convert to char array for the JSON decoder
      decodeJSON(responseBuffer);                      // Decode the JSON text
    }
  }
  client.stop();
}

bool skipResponseHeaders() { 
  char endOfHeaders[] = "\r\n\r\n"; // HTTP headers end with an empty line 
  client.setTimeout(HTTP_TIMEOUT); 
  bool ok = client.find(endOfHeaders); 
  if (!ok) { Serial.println("No response or invalid response!"); } 
  return ok; 
} 

bool decodeJSON(char *json) {
  StaticJsonBuffer <3*1024> jsonBuffer;
  char *jsonstart = strchr(json, '{'); // Skip characters until first '{' found and ignore length, if present
  if (jsonstart == NULL) {
    Serial.println("JSON data missing");
    return false;
  }
  json = jsonstart;
  JsonObject& root = jsonBuffer.parseObject(json); // Parse JSON
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }
  
  
    JsonObject& channel = root["feeds"][0]; // Now we can read 'feeds' values and so-on
    String entry_id     = channel["entry_id"];
    String field1value  = channel["field1"];
    Serial.print(" Field1 entry number ["+entry_id+"] had a value of: ");
    Serial.println(field1value);
     if(field1value.equals("1")) {
       digitalWrite(ledPin,HIGH);
    } else {
      digitalWrite(ledPin,LOW);
    }
 
}
