#include <Adafruit_MLX90614.h>
#include "MQ135.h"
//----
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <WiFiManager.h>
//----
#include <FirebaseArduino.h>

#define DIGITALPIN 0
#define FIREBASE_HOST "FIREBASE_HOST"
#define FIREBASE_AUTH "FIREBASE_AUTH"



MQ135 gasSensor = MQ135(DIGITALPIN);
String apiKey = "API key from ThingSpeak"; // Enter your Write API key from ThingSpeak


const char* server = "api.thingspeak.com";
WiFiClient client;

// -------------     Object Instance for MLX90614 tempeture sensor  ---------------------
Adafruit_MLX90614 termometroIR = Adafruit_MLX90614();

// -------------     /Object Instance for MLX90614 tempeture sensor  ---------------------



void setup() {
   // WiFiManager Instance creation.
  WiFiManager wifiManager;
 
  // Uncomment for configuration reset.
  wifiManager.resetSettings();
 
  // Make AP and portal cautivo.
  wifiManager.autoConnect("ESP8266Temp");
 
  Serial.println("You're connected.");  

  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  

  String Version = Firebase.getString("/Parent/Child");
  if (Firebase.failed())
  {
    Serial.print("Failure to conect firebase.");
    Serial.println(Firebase.error());        
  } else {
    Serial.println("Success firebase conection.");
    Serial.println(Version);       
  }
  
  termometroIR.begin();
}

void loop() {
  // Your code here 
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperaturaAmbiente = 0.0;
  float temperaturaAmbiente2 = 0.0;
  float test = 0.0;
  temperaturaAmbiente2 = termometroIR.readAmbientTempC();
  temperaturaAmbiente = termometroIR.readObjectTempC();  
  float rzero = gasSensor.getRZero();

   if (Firebase.getString("/Parent/Child") == "0") {
     // ----- "0"  Sensors Enabled
    Serial.println("Enabled Sensors");
    if (isnan(rzero) || isnan(temperaturaAmbiente || isnan(voltage)))
    {  
      Serial.println("Failed to read from temperaturaAmbiente ||  rzero sensor || voltage sensor");
      return;
    }
    
      float gasRzero = gasSensor.getRZero();
      float ilumination = voltage;
      float tempeture = temperaturaAmbiente;
      
      if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
      {
      
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(rzero);
        postStr += "&field2=";
        postStr += String(tempeture);
        postStr += "&field3=";
        postStr += String(ilumination);
        postStr += "&field4=";
        postStr += String(test);
        postStr += "r\n\r\n\r\n";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
      
        
        
        Serial.println("Data Send to Thingspeak-> Temperatura");
        Serial.print(tempeture);

        Serial.println();    
        Serial.println("Data Send to Thingspeak-> Oxigeno");
        Serial.print(rzero);
        
        Serial.println();    
        Serial.println("Data Send to Thingspeak-> Ilumination");
        Serial.println(ilumination);
        
        //---------------------------------------
      }
      client.stop();
      Serial.println("Waiting...");
      
      // thingspeak needs minimum 15 sec delay between updates.
      delay(1500);
        
    } else {
      // ------ "1" Sensors Disabled 
      Serial.println("Sensors Disabled");
      
      delay(2000);
   }
  
  
  
}
  
