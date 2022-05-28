#include <NfcAdapter.h>
#include <Servo.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

PN532_I2C pn532i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532i2c);
Servo myservo;  // create servo object to control a servo

#define LED_R 5
#define LED_G 6
#define trigPin 2
#define echoPin 3
#define BUZZER 7
#define SERVO 9

float inDoor_distance, distance_cm, door_distance;
unsigned long time_now = 0;
String correct = "F3 ED 58 90";
int pos = 0;

float measureDistance(){
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // calculate the distance
  return 0.017 * pulseIn(echoPin, HIGH);
}

void setup()
{
 Serial.begin(115200);
 //configure leds
 pinMode(LED_R, OUTPUT);
 pinMode(LED_G, OUTPUT);
 //configure buzzer
 pinMode(BUZZER, OUTPUT);
 noTone(BUZZER);
 //  configure servomotor
  myservo.attach(SERVO);
  myservo.write(pos);
  // configure the trigger pin to output mode
 pinMode(trigPin, OUTPUT);
 // configure the echo pin to input mode
 pinMode(echoPin, INPUT);
 Serial.println("TIMER1 Setup Finished.");
//  starting the nfc
 nfc.begin();
}

void loop()
{
  Serial.println("\nScan a NFC tag\n");
  // fading the led
  for(int i=0;i<=255;i++){
    analogWrite(LED_G,i);
    delay(2);
  }
  for(int i=255;i>=0;i--){
    analogWrite(LED_G,i);
    delay(2);
  }
  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    String uid = tag.getUidString();
    Serial.print("UID: ");Serial.println(uid);
    // the correct one is: F3 ED 58 90
    if(uid.equals(correct)){
      Serial.println("CORRECT!!");
      analogWrite(LED_R, 0);
      analogWrite(LED_G, 255);
      while(pos <= 90){
        pos++;
        myservo.write(pos);
        delay(5);
      }
      //wait 2 seconds for the door to open
      delay(2000);
      bool inDoor = false;
      bool passed = false;
      // calculate the distance of the open door
      door_distance = measureDistance();

      while(!passed)
      {
        // calculate the distance up the door
        distance_cm = measureDistance();

        // print the value to Serial Monitor
        Serial.print("distance: ");
        Serial.print(distance_cm);
        Serial.println(" cm");

        if(distance_cm < door_distance - 0.5)
        {
          while(!passed){
            inDoor_distance = measureDistance();
            if(inDoor_distance == distance_cm){
              passed = true;
            }
          }
          // wait 2 seconds for the person to pass the door
          // not to hit it
          delay(2000);
          while(pos >= 0){
            pos--;
            myservo.write(pos);
            delay(5);
          }
          inDoor = true;
        }
      }
      } else {
      analogWrite(LED_R, 255);
      analogWrite(LED_G, 0);
      //sound
      tone(BUZZER, 300, 1000);
      }
   }
  delay(1000);
  analogWrite(LED_R, 0);
}