#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern unsigned long pressTimes[10]; 

IPAddress apIP(55, 55, 55, 55);

const char* src_ssid = "ADGSH123"; 
ESP8266WebServer server(80);

void WifiSetup() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); // subnet FF FF FF 00
  WiFi.softAP(src_ssid);
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
}

void wifi_loop() {
  server.handleClient(); 
  delay(10);
}

void handleRoot() {
  String message = "<html><body><h1>Last 10 Button Presses</h1><table border='1'><tr><th>Press #</th><th>Time (ms)</th></tr>";
  for (int i = 0; i < 10; i++) {
    message += "<tr><td>" + String(i + 1) + "</td><td>" + String(pressTimes[i]) + "</td></tr>";
  }
  message += "</table></body></html>";
  server.send(200, "text/html", message);
}

void handleNotFound() {
  String message = "File Not Found \n \n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}


