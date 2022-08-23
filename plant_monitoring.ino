#include <SPI.h>
#include <Wire.h>
#include "DHT.h"  
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


const char* ssid = "chetan";  
const char* password = "1234567890";

DHT dht(2,DHT11);
const int dry = 805;  
const int wet = 380;  
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int motor = 13;
float Humidity;
float Temperature;
ESP8266WebServer server(80);

// Variable to store the HTTP request
String header;
String humidval;
String moistval;


void setup() {
  Serial.begin(115200); 
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop() 
{
  Serial.println("Temperature in C:");  
  Serial.println((dht.readTemperature( )));  
  Serial.println("Humidity in C:");  
  Serial.println((dht.readHumidity()));  
  delay(1000);
  soilMoistureValue = analogRead(SensorPin);  
  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, dry, wet, 0, 100);
  if(soilmoisturepercent > 100)
  {
    Serial.println("100 %");
    delay(400);
  }
  else if(soilmoisturepercent <0)
  {
    Serial.println("0 %");
    delay(400);
  }
  else if(soilmoisturepercent >=0 && soilmoisturepercent <= 100)
  {
    if(soilmoisturepercent >=0 && soilmoisturepercent <= 85)
    {
      Serial.println("pumping");
      digitalWrite(motor,HIGH);
      delay(3000);
      digitalWrite(motor,LOW);
    }
  Serial.print(soilmoisturepercent);
  Serial.println("%"); 
  delay(400);
  server.handleClient();
  }  
 }
 void handle_OnConnect() {

 soilmoisturepercent = map(soilMoistureValue, dry, wet, 0, 100); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  Temperature = dht.readTemperature();
  server.send(200, "text/html", SendHTML(soilmoisturepercent,Humidity,Temperature)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

 String SendHTML(float moiststat,float Humiditystat,float Tempstat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px; background-color: #F6E3C5} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 NodeMCU Weather Report</h1>\n";
  
  ptr +="<p>Moisture: ";
  ptr +=(int)moiststat;
  ptr +="%</p>";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";;//
  ptr +="<p>Temperature: ";
  ptr +=(int)Tempstat;
  ptr +="*C</p>";
  
  ptr +="</div>\n"; 
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
