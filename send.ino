#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ADGSH123";
const char* pswd = "";

extern WiFiClient client; 
int server_port = 80; 

void wifiClient_Setup() {
    Serial.begin(9600);
  Serial.println("wifiSetup");
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("trying ...");
    delay(100);
  }
  Serial.println("Connected to network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}




