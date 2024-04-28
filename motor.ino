#include <Servo.h>          //Servo motor library. This is standard library
#include <NewPing.h>        //Ultrasonic sensor function library. You must install this library
#include <Wire.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 0

char toesp= 'f';
//our L298N control pins
const int LeftMotorForward = 6;
const int LeftMotorBackward = 5;
const int RightMotorForward = 9;
const int RightMotorBackward = 3;


String lookingID = "";

//sensor pins
#define trig_pin A1 //analog input 1
#define echo_pin A2 //analog input 2

#define maximum_distance 200
boolean goesForward = false;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
byte nuidPICC[4];

int distance = 100;
int distance2 = 100;

NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function
NewPing sonar2(A0, A3, maximum_distance); //sensor function
Servo servo_motor; //our servo name


String printDec(byte *buffer, byte bufferSize) {
  String uid = "";
  for (byte i = 0; i < bufferSize; i++) {
//    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//    Serial.print(buffer[i], DEC);
    uid += buffer[i];
  }
  return uid;
}

void requestEvent() {
 Wire.write(toesp);  /*send string on request */
 
}

void receiveEvent(int howMany) {
 String id = "";
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    id += c;
  }
  lookingID = id;
  Serial.print("recieved");
  Serial.println(lookingID);
}

void setup(){

  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  Wire.begin(8);
  Wire.onReceive(receiveEvent); /* register receive event */
    Wire.onRequest(requestEvent);

  //Serial.begin(9600);
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);

  
  //servo_motor.attach(11); //our servo pin
}

String readID = "";
int speedL = 250;
int speedR = 250;
bool flag1 =false;
bool flag2 = false;

void loop(){

  toesp = 'f';
  if(lookingID.equals("")) {
    speedL = 0;
    speedR = 0;
    analogWrite(LeftMotorForward, speedL);
    analogWrite(RightMotorForward, speedR);
    return;
  }

  // Look for new cards
   rfid.PICC_IsNewCardPresent();
    //return;

  // Verify if the NUID has been readed
  //if ( ! 
  rfid.PICC_ReadCardSerial();
    //return;

  

  readID =  printDec(rfid.uid.uidByte,rfid.uid.size);
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  if(!readID.equals(""))
  {
    Serial.println(readID);
  }

  
  if(lookingID.equals(readID)) {
    speedL = 0;
    speedR = 0;
    analogWrite(LeftMotorForward, speedL);
    analogWrite(RightMotorForward, speedR);
    toesp = 't';
    //Wire.write(toesp);
    delay(2000);
    return;
  }

  //if(!flag1)
    speedL = 200;
  //if(!flag2)
    speedR = 200;
  
  distance = sonar.ping_cm();
  distance2 = sonar2.ping_cm();
  
  if (distance < distance2){
      speedR -= 40;
      Serial.println(speedR);  
      flag1 = true;
    }
    else flag1 = false;
  
    if(distance > distance2) {
      speedL -= 20;
      Serial.println(speedL);
      flag2 = true;
    }
    else flag2 = false;

  analogWrite(LeftMotorForward, speedL);
  analogWrite(RightMotorForward, speedR);
  
  analogWrite(LeftMotorBackward, 0);
  analogWrite(RightMotorBackward, 0);
  delay(100); 
}
