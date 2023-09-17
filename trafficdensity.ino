
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

const int trigPins[] = {3, 4, 6, 8};
const int echoPins[] = {2, 5, 7, 9};
int servoPins[] = {14, 15, 16, 17};

const int SS_PIN = 53;  
const int RST_PIN = 10;  

MFRC522 mfrc522(SS_PIN, RST_PIN);  

Servo servos[4];
unsigned long previousMillis = 0;
const long interval = 5000;  // in seconds

const int distanceThreshold = 10; // distance ultrasoni

boolean objectPresent = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();  
  mfrc522.PCD_Init();  

  for (int i = 0; i < 4; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    servos[i].attach(servoPins[i]);
    servos[i].write(90);  
  }
}

void loop() {
  boolean objectDetected = false;
  
  // Check RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("RFID Tag Detected");
    openGate(3);  //(0,1,2,3) gate numbers 
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1(); 
  }

  for (int i = 0; i < 4; i++) {
    long duration, distance;

    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);

    duration = pulseIn(echoPins[i], HIGH);
    distance = duration * 0.034 / 2;

    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance > 0 && distance < distanceThreshold) {
      objectDetected = true;
      if (!objectPresent) {
        objectPresent = true;
        openGate(i);
      }
    }
  }

  if (!objectDetected && objectPresent) {
    objectPresent = false;
    closeGates();
  }

  if (!objectPresent) {
    gateSequence();
  }
}

void openGate(int gateIndex) {
  for (int i = 0; i < 4; i++) {
    if (i == gateIndex) {
      servos[i].write(0);   // Open gate
    } else {
      servos[i].write(90);  // Close other gates
    }
  }
}

void closeGates() {
  for (int i = 0; i < 4; i++) {
    servos[i].write(90);  // Close all gates
  }
}

void gateSequence() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    static int currentGate = 0;

    closeGates();
    servos[currentGate].write(0);   
    currentGate = (currentGate + 1) % 4;
  }
}


//note_servo 0degree open gate for this project 90 close 
//4 servos 
//4 ultrasonic sensors 
//rfid modul 522
//componenst amount 2159rs only 


//projects.circuitseeker@gmail.com


