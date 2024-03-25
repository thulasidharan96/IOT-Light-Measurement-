#include <WiFi.h>
#include <ThingSpeak.h>
#include "DHTesp.h"
char ssid[] = "Wokwi-GUEST";
char pass[] = "";
WiFiClient client;
unsigned long myChannelNumber =  2072862;
const char * myWriteAPIKey = "FUPFO65KYKXFYZTJ";
DHTesp dhtSensor;
TempAndHumidity data;
const int DHT_PIN = 13;
const int TRIG_PIN = 2;
const int ECHO_PIN = 4;
long duration, distance;
int statusCode;
// Photoresistor Characteristics
const float GAMMA = 0.7;
const float RL10 = 50;


void setup() {
  Serial.begin(9600);
  pinMode(32, OUTPUT); 
WiFi.mode(WIFI_STA);
ThingSpeak.begin(client);
dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);
}

void loop() {
connectToCloud();
computeData();
writeData();

  int analogValue = analogRead(32);
  /*Reads the value from the specified analog pin. 
  In this case, the analog pin A0 is connected to the photoresistor sensor */
  
  float voltage = analogValue / 1024. * 5;

  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  /*various calculations to get the luminosity value from the
  photoresistor sensor */
  Serial.println("-----------");
 


  if (lux > 50) {
      Serial.println("Light!");
      Serial.println("Luminosity: " + String(lux));
      digitalWrite(10, LOW);
       //if the luminosity is greater than 50, turn off the LED
  } else {
    Serial.println("Dark!");
    Serial.println("Luminosity: " + String(lux));
    digitalWrite(10, HIGH);
    //else, turn on the LED
  }
delay(5000);
    
}

void connectToCloud(){
if(WiFi.status() != WL_CONNECTED) {
Serial.print("Attempting to connect");
while(WiFi.status() != WL_CONNECTED) {
WiFi.begin(ssid, pass);
for(int i=0;i<5;i++) {
Serial.print(".");
delay(5000);
}
}
Serial.println("\nConnected.");
}
}
void computeData(){
distance = getDistanceInCentimeters();
data = dhtSensor.getTempAndHumidity();
Serial.println("Humi: " + String(data.humidity));
Serial.println("Temp: " + String(data.temperature));
Serial.println("Dist: " + String(distance));

Serial.println("-----------");
}
void writeData(){
ThingSpeak.setField(1, distance);
ThingSpeak.setField(3, data.humidity);
ThingSpeak.setField(2, data.temperature);
ThingSpeak.setField(4, (lux));
statusCode = ThingSpeak.writeFields(myChannelNumber,myWriteAPIKey);
if(statusCode == 200) //successful writing code
Serial.println("Channel update successful.");
else
Serial.println("Problem Writing data. HTTP error code :" +
String(statusCode));
delay(5000); // data to be uploaded every 15secs
}
long getDistanceInCentimeters() {
digitalWrite(TRIG_PIN,LOW);
delayMicroseconds(2);
digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);
duration = pulseIn(ECHO_PIN, HIGH);
return (duration * 0.034 / 2);
}