#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;
WidgetLED led(V0);

char auth[] = "48-phmE74JWzjDtPMzF5FUNU6DjKH7FF";
char ssid[] = "God Is Always Near";
char pass[] = "None1234";
WiFiClient  client;
unsigned long myChannelNumber = 1132486;
const char * myWriteAPIKey = "W9JJUTO4FHQ19QC1";

int buzzer = D0, relay = D3, trig = D1, echo = D2, ir = D5;
int irval = 1, dist, distance = 0, count = 0, duration;

void sanitizer_on(){
  irval = digitalRead(ir);
  Serial.println("On: "+String(irval));
  if(irval == 0){
    digitalWrite(relay, LOW);
    digitalWrite(buzzer, LOW);
    count = count + 1;
    led.on();
  }
  else{
    digitalWrite(relay, HIGH);
    digitalWrite(buzzer, HIGH);
    led.off();
  }
  Blynk.virtualWrite(V1, count);
}

void sanitizer_off(){
  irval = digitalRead(ir);
  Serial.println("Off: "+String(irval));
  if(irval == 1){
    digitalWrite(relay, HIGH);
    digitalWrite(buzzer, HIGH);
    led.off();
  }
  irval = 1;
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  duration = pulseIn(echo, HIGH);
  dist = duration * 0.034/2;
  Serial.println("Distance: "+String(dist)+"cm");
  distance = map(dist, 2, 250, 100, 0);
  Serial.println("Distance in %: "+String(distance)+"%");
  Blynk.virtualWrite(V2, distance);
  if(distance < 20){
    Blynk.notify("There is no Sanitizer left");
  }
}

void thingspeak(){  
  ThingSpeak.setField(1, irval);
  ThingSpeak.setField(2,distance);
 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(ir, INPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  timer.setInterval(3000, sanitizer_off);
  timer.setInterval(10000, sanitizer_on);
  timer.setInterval(1000, thingspeak);
}

void loop()
{
  Blynk.run();
  timer.run();
}
