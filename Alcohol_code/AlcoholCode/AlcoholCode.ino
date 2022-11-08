
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#define BLYNK_PRINT Serial    


char auth[] = "I7n_laKD-tatm40ilt6IZTwkDmiLyW4C"; 
char ssid[] = "Dialog 4G"; 
char pass[] = "TQ04BGLLR0L"; 
 
SimpleTimer timer;
 
int mq2 = A0; 
int data = 0; 
void setup() 
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, getSendData);
}
 
void loop() 
{
  timer.run(); 
  Blynk.run();
}
 
void getSendData()
{
data = analogRead(mq2); 
  Blynk.virtualWrite(V2, data);
 
  if (data > 700 )
  {
    Blynk.notify("Alcohol Detected!"); 
  }
 
}
