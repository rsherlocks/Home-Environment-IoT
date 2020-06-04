#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"
#include <NTPClient.h>
#include <Wire.h> 

//#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
// change next line to use with another board/shield
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 14   
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "pool.ntp.org", 21600, 21600);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

// Set these to run example.
#define FIREBASE_HOST "smartproject-3b838.firebaseio.com"
#define FIREBASE_AUTH "ePYj1KAEEsgTevUgz63xGN0obNSY2clyCd8nDv77"
#define WIFI_SSID "AndroidShaper"
#define WIFI_PASSWORD "b@ngl@desh54321"
String myString;
String myString1;
String myString2;
void setup() {
   Serial.begin(115200);
   lcd.begin();
   dht.begin();
   pinMode(D7, OUTPUT);
   pinMode(D8, OUTPUT);
    pinMode(D4, OUTPUT);
    digitalWrite(D4, 1);
 
 Serial.println();
  // connect to wifi.

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting ");
    lcd.setCursor(0, 0);
    lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // Wait for serial to initialize.
  lcd.setCursor(0, 0);
  lcd.print("Conneccted!  ");
  delay(5000);
  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

}
int timeSinceLastRead = 0;
int rel=0;

void loop() {
  if(rel==0)
  {
    digitalWrite(D4, 1);
  }
  
   digitalWrite(D8, HIGH);
   while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
   Serial.println(formattedDate);
  if (Firebase.failed()) {
      Serial.print("setting / failed:");
      Serial.println(Firebase.error());  
  
  }
  

  float h = dht.readHumidity();
  
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

   float thi=(0.8*t + (h/100)*(t-14.4) + 46.4);
   thi=round(thi)-3;
   myString = String(t); 
myString1 = String(h);
myString2 = String(thi);
lcd.setCursor(0, 1);
  lcd.print("THI: ");
  lcd.setCursor(4, 1);
  lcd.print(myString2);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print("%\t");
    Serial.print("Temperature Humidity Index: ");
    Serial.print(thi);
  Serial.println();
    if(thi>=78 && rel==0)
  {
 
  digitalWrite(D7, HIGH);
  digitalWrite(D4, 0);
   lcd.setCursor(0, 0);
   if(thi>=78 && thi<=89)
   {
    lcd.print("Severe Stress!  ");
    lcd.setCursor(12, 1);
    lcd.print("ON ");
   }
   if(thi>=89 && thi<=98)
   {
    lcd.print("Very Severe Stress!");
    lcd.setCursor(12, 1);
    lcd.print("ON ");
   }
   if(thi>=99)
   {
    lcd.print("Dead Cows!      ");
    lcd.setCursor(12, 1);
    lcd.print("ON ");
   }
 
  rel=1;
 
  }
  if(thi<78){
    digitalWrite(D7, LOW);
    digitalWrite(D4, 1);
    lcd.setCursor(0, 0);
    if(thi>=72 && thi<78)
    {
      lcd.print("Mild Stress!    ");
     lcd.setCursor(12, 1);
     lcd.print("OFF");
     
    }
    if(thi<72)
    {
      lcd.print("No Heat Stress!");
     lcd.setCursor(12, 1);
     lcd.print("OFF");
     
    }
  
    rel=0;
  }
//  if (Firebase.failed()) {
//      Serial.print("setting /number failed:");
//      Serial.println(Firebase.error());  
//      setup();
//  }
//   Firebase.setFloat("TEMPERATURES1", dht.readTemperature());
//    Firebase.setFloat("HUMIDITIES1", dht.readHumidity());
//     Firebase.setFloat("THI", thi);

Firebase.setString("Farm/TEMPERATURES",myString);
Firebase.setString("Farm/HUMIDITIES",myString1);
Firebase.setString("Farm/THI",myString2);
Firebase.setString("Farm/Dtime",formattedDate);
if(timeSinceLastRead>=60)
{
Firebase.pushInt("Farm/LogTem", t);
Firebase.pushInt("Farm/LogHum", h);
Firebase.pushInt("Farm/LogThi", thi);
Firebase.pushString("Farm/Dtimes", formattedDate);
timeSinceLastRead = 0;
  
}


timeSinceLastRead += 1;
delay(60000);

}
