#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include<LiquidCrystal_I2C.h>
#include <Servo.h>

Servo entrance;
int lcdColumns = 16;
int lcdRows = 2;
int sensor1 = 16; 
int sensor2 = 0; 
int sensor3 = 14;
int servoEntrance = 12; 
int i = 0;
boolean closed = false;
int leftSpace;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
const char *ssid = "safari";
const char *password = "1234567890";
  
void setup() {
  pinMode(sensor1,INPUT_PULLUP);
  pinMode(sensor2,INPUT_PULLUP);
  pinMode(sensor3,INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  entrance.attach(servoEntrance);
  entrance.write(0);
  delay(1000);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.print("Connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to "); 
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Occupancy: ");
  WiFiClientSecure client; // initialize client object
  HTTPClient http; // initialize HTTPClient object to use our nodeMCU as a client not server
  client.setInsecure();
  if (http.begin(client, "https://bidirectional-visitor-counter.herokuapp.com/api/v1/get-entries")) {  // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          String remains = getValue(payload, '/', 0);
          leftSpace = remains.toInt();
          if(leftSpace >= 15){
            for(; i>=0; i--){
                entrance.write(i);
                delay(15);
              }
              closed = true;
              
            } if(leftSpace < 15){
                 for(; i<=120; i++){
                entrance.write(i);
                delay(15);
              }
              closed = false;
            }
          lcd.setCursor(0,1);
          lcd.print(payload);
          lcd.print("       ");
        }
      } else {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Failed :(");
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Unable to Connect");
      Serial.printf("[HTTP} Unable to connect\n");
    }
  
  //==================SENSOR ONE=======================================
  if(digitalRead(sensor1) == LOW && closed == false){
     String Link1;
  
  Link1 = "https://bidirectional-visitor-counter.herokuapp.com/api/v1/increment"; // API to change status for parking space 1 on hosted database 
  
  http.begin(client, Link1);     //Specify request destination
  
  int httpCode1 = http.GET();      
  if(httpCode1 > 0)
  {
      if(httpCode1 == HTTP_CODE_OK){
        String payload1 = http.getString();
        Serial.println(payload1);
      }else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode1).c_str());
      }
  }else{
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode1).c_str());//Print request response payload
  }
  while(digitalRead(sensor1) == LOW){
    yield(); 
  }
  }


  //=================SENSOR TWO==============================================
  if(digitalRead(sensor2) == LOW && closed == false){
      String Link2;
  
  Link2 = "https://bidirectional-visitor-counter.herokuapp.com/api/v1/increment"; // API to change status for parking space 2 on hosted database
  
  http.begin(client, Link2);     //Specify request destination
  
  int httpCode2 = http.GET();      
  if(httpCode2 > 0)
  {
      if(httpCode2 == HTTP_CODE_OK){
        String payload2 = http.getString();
        Serial.println(payload2);
      }else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode2).c_str());
      }
  }else{
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode2).c_str());//Print request response payload
  }
    while(digitalRead(sensor2) == LOW){
      yield();
    }
  }

  //=================SENSOR THREE==============================================
  if(digitalRead(sensor3) == LOW){
    String Link3;
  
  Link3 = "https://bidirectional-visitor-counter.herokuapp.com/api/v1/decrement"; // API to change status for parking space 3 on hosted database
  
  http.begin(client, Link3);     //Specify request destination
  
  int httpCode3 = http.GET();      
  if(httpCode3 > 0)
  {
      if(httpCode3 == HTTP_CODE_OK){
        String payload3 = http.getString();
        Serial.println(payload3);
      }else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode3).c_str());
      }
  }else{
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode3).c_str());//Print request response payload
  }
    while(digitalRead(sensor3) == LOW){
      yield();
    }
  }
  
}
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//=======================================================================
