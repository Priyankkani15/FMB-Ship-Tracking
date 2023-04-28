#define BLYNK_TEMPLATE_ID "TMPL3xsKpKPjX"
#define BLYNK_DEVICE_NAME "BORDER ALERT"
#define BLYNK_AUTH_TOKEN "wJTXPfuJFwz92JQMDldXoAhGmA2IegrG"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "iot12345";
char pass[] = "iot12345";
BlynkTimer timer;
#include <TinyGPS++.h>
TinyGPSPlus gps;
#define RXD2 13
#define TXD2 12
#define RXD1 14
#define TXD1 27
WidgetLED p2(V2);
const String PHONE = "+916383485969";
unsigned long previousMillis=0;
unsigned long int previoussecs = 0; 
unsigned long int currentsecs = 0; 
 unsigned long currentMillis = 0; 
 int secs = 0; 
 int pmints = 0; 
 int mints = 0; // current mints
 int interval= 1 ; // updated every 1 second
const float maxDistance = 5;
float initialLatitude =11.0165;
float initialLongitude = 76.9644;
float latitude, longitude;

void setup() 
{
 Serial.begin(9600);
 Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
 Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
 Blynk.begin(auth, ssid, pass);
 pinMode(25,OUTPUT);
  Serial1.println("AT"); //Check GSM Module
  delay(1000);
  Serial1.println("ATE1"); //Echo ON
  delay(1000);
  Serial1.println("AT+CPIN?"); //Check SIM ready
  delay(1000);
  Serial1.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  Serial1.println("AT+CNMI=1,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  
}

void loop() 
{
    Blynk.run();
     while (Serial2.available() > 0) { // Read GPS data if available
    if (gps.encode(Serial2.read())) { // Parse GPS data
      if (gps.location.isValid()) { // Check if GPS data is valid
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6); // Print latitude with 6 decimal places
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6); // Print longitude with 6 decimal places
       latitude=gps.location.lat();longitude=gps.location.lng(); 
    Blynk.virtualWrite(V0,latitude);
    Blynk.virtualWrite(V1,longitude);
      }
    }
  }
  float distance = getDistance(latitude, longitude, initialLatitude, initialLongitude);
  Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6); // Print latitude with 6 decimal places
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6); // Print longitude with 6 decimal places
  Serial.print("initialLatitude= "); Serial.println(initialLatitude, 6);
  Serial.print("initialLngitude= "); Serial.println(initialLongitude, 6);
  Serial.print("current Distance= "); Serial.println(distance);
  if(distance > maxDistance) {
      p2.on();
      digitalWrite(25, HIGH);
      delay(1000);
      sendAlert();
      delay(10000);
  }
  if(distance < maxDistance){
    p2.off();
    digitalWrite(25, LOW);
     delay(1000); }
}

float getDistance(float flat1, float flon1, float flat2, float flon2) {

  // Variables
  float dist_calc=0;
  float dist_calc2=0;
  float diflat=0;
  float diflon=0;

  // Calculations
  diflat  = radians(flat2-flat1);
  flat1 = radians(flat1);
  flat2 = radians(flat2);
  diflon = radians((flon2)-(flon1));

  dist_calc = (sin(diflat/2.0)*sin(diflat/2.0));
  dist_calc2 = cos(flat1);
  dist_calc2*=cos(flat2);
  dist_calc2*=sin(diflon/2.0);
  dist_calc2*=sin(diflon/2.0);
  dist_calc +=dist_calc2;

  dist_calc=(2*atan2(sqrt(dist_calc),sqrt(1.0-dist_calc)));
  
  dist_calc*=6371000.0; //Converting to meters

  return dist_calc;
}

void sendAlert()
{
  //return;
  String sms_data;
  sms_data = "Alert! outside the GEOfence.\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += String(latitude) + "," + String(longitude);

  //return;
  Serial1.print("AT+CMGF=1\r");
  delay(1000);
  Serial1.print("AT+CMGS=\""+PHONE+"\"\r");                                                                           
  delay(1000);
  Serial1.print(sms_data);
  delay(100);
  Serial1.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial1.println("SMS Sent Successfully.");
}
