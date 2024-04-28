
//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <EduIntro.h>
#include <Wire.h>

#define FIREBASE_HOST "https://xxxxxxx"
#define FIREBASE_AUTH "xxxxxxxZCoYxxxxxjttSvxxxxxMHuwxxxxxiMJH"
#define WIFI_SSID "xxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxx"


//Define FirebaseESP8266 data object
FirebaseData firebaseData;

String finding = "";
String oldFinding = "h";

void setup()
{
  //pinMode(5,INPUT_PULLUP);
  Serial.begin(9600);

  Serial.print("Connecting to Wi-Fi");

  Wire.begin(5, 4);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop()
{

  if (Firebase.getString(firebaseData, "/search")) {
    finding = firebaseData.stringData();
  }
  else {
    Firebase.setString(firebaseData, "status", "id not found");
    return;
  }

  if (Firebase.getString(firebaseData, "/instructors/" + finding)) {

    String id = firebaseData.stringData();
//    Serial.println(id);
//    Serial.println(id.length());

    if (oldFinding.equals(finding))
    {
      return;
    }
    Firebase.setString(firebaseData, "status", "Kart is searching");

    char farray [id.length()];
    id.toCharArray(farray, id.length() + 1);
    Serial.print(farray);
    Wire.beginTransmission(8); /* begin with device address 8 */
    Wire.write(farray);  /* sends hello string */
    Wire.endTransmission();    /* stop transmitting */
    //    //Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
    //    while (Wire.available()) {
    //      char c = Wire.read();
    //      Serial.print(c);
    //    }
    Serial.println();
    delay(1000);
    waitResponse();
    oldFinding = finding;
  }
  else {
    Firebase.setString(firebaseData, "status", "instructor not found");
  }

  delay(500);
}

void waitResponse()
{
  //Wire.requestFrom(8, 13);
  char ran = 'f';
  //while (0 == Wire.available()) {
  while (ran == 'f') {
    Serial.println(Wire.available());
    delay(500);
    Wire.requestFrom(8, 1);
    while (Wire.available()) {
      char c = Wire.read();
      ran = c;
    }
  }

  Serial.println("i'm back");
//  while (Wire.available()) {
//    char c = Wire.read();
//    Serial.print(c);
//  }
  Firebase.setString(firebaseData, "status", "Kart in correct location");
}
