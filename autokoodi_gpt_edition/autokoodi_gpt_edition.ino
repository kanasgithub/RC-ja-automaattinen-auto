#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
#include <ESP32Servo.h>

Servo silma; //tekee servon
int motordelay = 550; //asettaa viiveen moottori käännös funktioille
int IRpin = 35;
float IRT; //muuttuja IR sensorin arvolle
int IRsens = 1400;
int rcservo = 90;
int timeout; //timeout jos robotti jumissa
int sonarstoragetimer = 10; //tallenus ajastin luotaimen tallennukseen
float sonarstorage;
int hasreversed = 0;

int emergencyrev = 0;

int scandelay = 300; //muokkaa delayta kaikuluotauksen kääntymisellä sonar funktioissa
int rctoggle = 1; //käytetään moodin vaihtoon käynnistyksessä
int vasentaakse = 27; //pinnit moottorien suunnille
int vaseneteen = 26;
int oikeataakse = 33;
int oikeaeteen = 25;
char SBT; //SBT muuttuja bluetoothin käytölle
float duration, distance, left, middle, right; //muuttujat luotaimen käyttöön
int trig = 14; //kaikuluotain trigger pinni
int echopin = 34; //kaikuluotain vastaanotto pinni
float drivestop; //pysäytys arvon muuttuja
int startswitch = 1; //käynnistysnapin muuttuja

void setup() {
  pinMode(16, INPUT_PULLUP);
  rctoggle = digitalRead(16);
  pinMode(trig, OUTPUT);
  pinMode(echopin, INPUT);

  silma.attach(13);

  pinMode(27, OUTPUT); // Right wheel back
  pinMode(26, OUTPUT); // Right wheel forward
  pinMode(33, OUTPUT); // Left wheel back
  pinMode(25, OUTPUT); // Left wheel forward

  pinMode(IRpin, INPUT);

  // Make sure motors are off at the start
  digitalWrite(vaseneteen, LOW);
  digitalWrite(oikeaeteen, LOW);
  digitalWrite(vasentaakse, LOW);
  digitalWrite(oikeataakse, LOW);

  Serial.begin(115200);
  if (rctoggle == 0) {
    SerialBT.begin("RC auto");// Bluetooth device name
  }
}
void remotecontrol()
{
  // Check if there's incoming Bluetooth data
  if (SerialBT.available()) {
    SBT = SerialBT.read(); // Read single character command
    Serial.println(SBT); // Print received command for debugging
    delay(20);

    // Koodi lukee SBTn ja tekee komennot riippuen mitä siinä on
    if (SBT == 'f') { /
      digitalWrite(vaseneteen, HIGH);
      digitalWrite(oikeaeteen, HIGH);
      digitalWrite(vasentaakse, LOW);
      digitalWrite(oikeataakse, LOW);
    }
    else if (SBT == 'r') {
      digitalWrite(vaseneteen, HIGH);
      digitalWrite(oikeaeteen, LOW);
      digitalWrite(vasentaakse, LOW);
      digitalWrite(oikeataakse, HIGH);
    }
    else if (SBT == 'l') {
      digitalWrite(vasentaakse, HIGH);
      digitalWrite(oikeaeteen, HIGH);
      digitalWrite(vaseneteen, LOW);
      digitalWrite(oikeataakse, LOW);
    }
    else if (SBT == 'b') {
      digitalWrite(vasentaakse, HIGH);
      digitalWrite(oikeataakse, HIGH);
      digitalWrite(vaseneteen, LOW);
      digitalWrite(oikeaeteen, LOW);
    }
    else if (SBT == 's') {
      digitalWrite(vasentaakse, LOW);
      digitalWrite(oikeataakse, LOW);
      digitalWrite(vaseneteen, LOW);
      digitalWrite(oikeaeteen, LOW);
    }

  }
}

//luotaimet ja niiden suuntaus joka kääntää servoa ja asettaa arvot muuttujiin
void sonarmid() //middle
{ silma.write(90);
  delay(scandelay);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echopin, HIGH);
  middle = (duration * .0343) / 2;
  Serial.print("keski");
  Serial.println(middle);
  delay(100);

}
//funktiot kaikuluotaimeen ja sen muuttujiin, alemmat muuttujat pätevät myös toiseen luotain funktioon
void sonarleft() //left
{ silma.write(167); //servo kulma
  delay(scandelay); //skannaus viive jotta servo voi liikkua
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echopin, HIGH);
  left = (duration * .0343) / 2;
  Serial.print("vasen");
  Serial.println(left);
  delay(100);
}

void sonarright() //right
{
  silma.write(25);
  delay(scandelay);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echopin, HIGH);
  right = (duration * .0343) / 2;
  Serial.print("oikea");
  Serial.println(right);
  delay(100);
}
//funktiot liikkumiseen
void tleft() {
  digitalWrite(oikeaeteen, HIGH);
  digitalWrite(vasentaakse, HIGH);
  delay(motordelay);
  digitalWrite(oikeaeteen, LOW);
  digitalWrite(vasentaakse, LOW);
  emergencyrev ++;
}

void tright() {
  digitalWrite(vaseneteen, HIGH);
  digitalWrite(oikeataakse, HIGH);
  delay(motordelay);

  digitalWrite(vaseneteen, LOW);
  digitalWrite(oikeataakse, LOW);
  emergencyrev ++;
}
void drive() {
  drivestop = 100;
  IRT = analogRead(IRpin);
  while (drivestop >= 35 &&  IRT <= IRsens) {
    digitalWrite(vaseneteen, HIGH);
    digitalWrite(oikeaeteen, HIGH);
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    duration = pulseIn(echopin, HIGH);
    drivestop = (duration * .0343) / 2;
    Serial.print("etäisyys");
    Serial.println(drivestop);
    IRT = analogRead(IRpin);
    Serial.println(IRT);
    Serial.println("IR SENSORIIIIIIIIIIIII");
    /*
      Serial.println(sonarstoragetimer);
      Serial.print("sonar timer");
       sonarstoragetimer = sonarstoragetimer - 1;
       if (sonarstoragetimer ==  4) {
         sonarstorage = (duration * .0343) / 2;
       }

       if (sonarstorage >= drivestop && sonarstorage == 0) {
         sonarstoragetimer = 10;
         drivestop = 30;

       } */
    delay(100);

  }
  digitalWrite(vaseneteen, LOW);
  digitalWrite(oikeaeteen, LOW);
}
void sonarcycle() {
  sonarleft();
  sonarmid();
  sonarright();
  delay(100);
  silma.write(90);
}
void reverse() {
  digitalWrite(vasentaakse, HIGH);
  digitalWrite(oikeataakse, HIGH);
  delay(1000);
  digitalWrite(vasentaakse, LOW);
  digitalWrite(oikeataakse, LOW);
  emergencyrev = 0;
  hasreversed = 1;
}

void loop() {
  if (rctoggle == 0) {
    remotecontrol();
  }
  else { //pikku toggle joka lukee moodinappia ja tarkistaa jos se menee bluetooth tilaan vai automaatille. toimii myös käynnistysnappina automaatti tilassa
    if (startswitch == 1) {
      while (startswitch == 1) {
        startswitch = digitalRead(16);
      }
    }


    sonarcycle();
    IRT = analogRead(IRpin);
    if (middle < 30 || IRT >= IRsens || emergencyrev == 5) {
      reverse();
    }
    /*  else {
       if (IRT >= IRsens || emergencyrev == 5) {
         reverse();
       } */
    else {

      if (35 < middle && hasreversed == 0) { //jos muutat etäisyyttä muista vuokata void drive() funktion etäisyys rajaa!
        Serial.println("ETEEN!!!!");
        drive();
      }
      
      if (left > right && middle < 33 || left > right && hasreversed == 1 ) {
        tleft();
        Serial.println("VASEMMALLE!!!!");
        hasreversed = 0;
      }
      
      if (left < right && middle < 35 || left < right && hasreversed == 1 ) {
        tright();
        Serial.println("OIKEALLE!!!!");
        hasreversed = 0;
      }

      delay(300);

    }
  }
  Serial.println(IRT);
  Serial.println("IR SENSORIIIIIIIIIIIII");
  Serial.println(hasreversed);
  Serial.print("pakkistatus");

}
