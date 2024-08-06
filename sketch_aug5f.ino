#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define btnPin D5
#define redLedPin D2
#define greenLedPin D1
#define blueLedPin D3 

int lastVal;
unsigned long LastPressTime;
unsigned long pressTimes[10]; 
int pressIndex = 0;

WiFiClient client; 

void setup() {
  Serial.begin(9600); 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT); // הגדרת פין כחול כיציאה

  LastPressTime = millis();
  lastVal = digitalRead(btnPin);

  // wifiClient_Setup();
  WifiSetup(); 
}

void loop() {
  wifi_loop();
  int val = digitalRead(btnPin);

  if ((val == LOW) && (lastVal == HIGH) && (millis() - LastPressTime > 50)) {
    LastPressTime = millis();
    SendBtnPressed(LastPressTime);
    recordPress(LastPressTime);
  }

  lastVal = val;

  checkAndUpdateServer();
  wifi_loop(); 
  delay(1000); 
}

void checkAndUpdateServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, "http://api.kits4.me/GEN/api.php?ACT=GET&DEV=11&CH=1");
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      int serverValue = payload.toInt();

      if (LastPressTime < serverValue) {
        setLEDColor(0, 255, 255); 
        String url = "http://api.kits4.me/GEN/api.php?ACT=SET&DEV=11&CH=1&VAL=" + String(LastPressTime);
        http.begin(client, url);
        int setHttpCode = http.GET();

        if (setHttpCode > 0) {
          Serial.println("Server updated successfully");
        } else {
          Serial.println("Failed to update server");
        }
      } else {
        setLEDColor(255, 100, 0); // צבע כתום
      }
    } else {
      Serial.println("Failed to retrieve data from server");
    }

    http.end();
  }
}

void setLEDColor(int red, int green, int blue) { 
  analogWrite(redLedPin, red);
  analogWrite(greenLedPin, green);
  analogWrite(blueLedPin, blue); 
}

void recordPress(unsigned long pressTime) {
  pressTimes[pressIndex] = pressTime;
  pressIndex = (pressIndex + 1) % 10; 
}

void SendBtnPressed(unsigned long pressTime) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.kits4.me/GEN/api.php?ACT=SET&DEV=11&CH=1&VAL=" + String(pressTime);
    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Press time sent to server");
    } else {
      Serial.println("Failed to send press time to server");
    }

    http.end();
  }
}




