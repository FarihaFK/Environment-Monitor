#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <ThingSpeak.h>


SoftwareSerial mySerial(D3, D4); //SIM800L Tx & Rx is connected to ESP8266 D3, D4
DHT dht(D2, DHT11); // check dht pin no. !!  D2
const int aqsensor = A0;  //output of mq135 connected to A0 pin of NodeMCU
int flag=0;
const char *ssid = "Samiha Tahsin"; // Enter your WiFi Name
const char *pass = "pongpong"; // Enter your WiFi Password

WiFiClient client;

long myChannelNumber = 1822810;   // change!!
const char myWriteAPIKey[] = "SFXPNSH9UUBDQR5L";  // change!!

void setup() {
  // put your setup code here, to run once:
  pinMode (aqsensor,INPUT);   // MQ135 is connected as INPUT to ESP8266

  Serial.begin(115200);
  delay(100);// baud rate !!
  mySerial.begin(115200);
  WiFi.begin(ssid, pass); // change to your wifi password
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print("Connecting to wifi..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  dht.begin();
  delay(3000);
  ThingSpeak.begin(client);

   mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
 mySerial.println("AT+CMGS=\"01685441462\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

  int g = analogRead(aqsensor); //read MQ135 analog outputs at A0 and store it in ppm
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  Serial.print("Air Quality: ");  //print message in serail monitor
  Serial.println(g);            //print value of ppm in serial monitor
  Serial.print(" ppm");
  if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
   else
                 {
 
                     Serial.print("Temperature: ");
                     Serial.print(t);
                     Serial.print(" degrees Celcius, Humidity: ");
                     Serial.print(h);
                 }
  if(!flag) // condition needs to be modified !!!!!
  {
      mySerial.print("air quality: "); //text content
      updateSerial();
      mySerial.print(g); //text content
      updateSerial();
      mySerial.print("  ppm \n Temperature: "); //text content
      updateSerial();
      mySerial.print(t); //text content
      updateSerial();
      mySerial.print("degree,\n Humidity : "); //text content
      updateSerial();
      mySerial.print(h); //text content
      updateSerial();
      mySerial.write(26);
      
      flag=1;
    }
  delay(1000); 
  
  ThingSpeak.writeField(myChannelNumber, 1, g, myWriteAPIKey); 
  ThingSpeak.writeField(myChannelNumber, 2, t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3, h, myWriteAPIKey);
  delay(2000);
  
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
