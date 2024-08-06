#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define btnPin D5
#define redPin D7
#define greenPin D6

int lastVal;
unsigned long LastPressTime;
unsigned long pressTimes[10]; 
int pressIndex = 0;

WiFiClient client; 
void setup() {
  // Serial.begin(9600); // 
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

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
        setLEDColor(0, 255); // תכלת טורקיז )(אין לי כצבע כחול נעשה עם ירוק ואדום)
        String url = "http://api.kits4.me/GEN/api.php?ACT=SET&DEV=11&CH=1&VAL=" + String(LastPressTime);
        http.begin(client, url);
        int setHttpCode = http.GET();

        if (setHttpCode > 0) {
          Serial.println("Server updated successfully");
        } else {
          Serial.println("Failed to update server");
        }
      } else {
        setLEDColor(255, 100); // כתום
      }
    } else {
      Serial.println("Failed to retrieve data from server");
    }

    http.end();
  }
}

void setLEDColor(int red, int green) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
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



