
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#include "DHT.h"

#define DHTPIN 4 // what digital pin we're connected to NodeMCU (D1)
#define DHTTYPE DHT22 // DHT 11


DHT dht(DHTPIN, DHTTYPE);

char str_hum[16];
char str_temp[16];
char str_hum1[16];
char str_temp1[16];
const int buzzer = 5;//BUZZER
const int led = 0;


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Xperia XZ Premium"
#define WLAN_PASS       "1sampai10"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "--------"
#define AIO_KEY         "----------"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp_cfu");
Adafruit_MQTT_Publish chiller1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/max_cfu");
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void Buzzer(){
  tone(buzzer,2200);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  digitalWrite(led,HIGH);
}


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();


  
      // Wait a few seconds between measurements.
      delay(2000);
      
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

      
      //SENSOR CHILLER 1
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
      }
      
//      // SENSOR CHILLER 2
//       float h1 = dht1.readHumidity();
//      // Read temperature as Celsius (the default)
//      float t1 = dht1.readTemperature();
//      // Read temperature as Fahrenheit (isFahrenheit = true)
//      float f1 = dht1.readTemperature(true);
//
//       // Check if any reads failed and exit early (to try again).
//      if (isnan(h1) || isnan(t1) || isnan(f1)) {
//      Serial.println("Failed to read from DHT sensor!");
//      return;
//      }
      
      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print("Temperature Chiller 1 in Celsius:");
      Serial.println(String(t).c_str());

//      // Compute heat index in Fahrenheit (the default)
//      float hif1 = dht1.computeHeatIndex(f1, h1);
//      // Compute heat index in Celsius (isFahreheit = false)
//      float hic1 = dht1.computeHeatIndex(t1, h1, false);
//      Serial.print("Temperature: ");
//      Serial.print(t1);
//      Serial.print(" *C ");
//      Serial.print("Temperature Chiller 2 in Celsius:");
//      Serial.println(String(t1).c_str());
//
//      

    //dtostrf(gps_latitude, 4, 2, str_temp);
    //dtostrf(gps_longitude, 4, 2, str_hum);

  // Now we can publish stuff!

  Serial.print(F("\nSending Temperature value: "));
  Serial.print(String(t).c_str());
  Serial.print("...");
  if (! temp.publish(String(t).c_str())) {
    Serial.println(F("Failed"));
    delay(200);
  } else {
    Serial.println(F("OK!"));
    delay(5000);
  }
  if(t > 8 ){
    chiller1.publish(String(t).c_str());
      Buzzer();   // turn the LED on (HIGH is the voltage level)
  }else if (t < 2 ){
    chiller1.publish(String(t).c_str());
      Buzzer();   // turn the LED on (HIGH is the voltage level)
  }else{
    Serial.println("Suhu aman !");
  }


//    Serial.print(F("\nSending Temperature value: "));
//  Serial.print(String(t1).c_str());
//  Serial.print("...");                                                                                    f
//  if (! temp2.publish(String(t).c_str())) {
//    Serial.println(F("Failed"));
//    delay(200);
//  } else {
//    Serial.println(F("OK!"));
//    delay(5000);
//  }
//  
//
//  if(t > 8 ){
//    chiller2.publish(String(t).c_str());
//       // turn the LED on (HIGH is the voltage level)
//  }else if (t < 2 ){
//    chiller2.publish(String(t).c_str());
//        // turn the LED on (HIGH is the voltage level)
//  }else{
//    Serial.println("Suhu aman !");
//  }
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  delay(5000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
