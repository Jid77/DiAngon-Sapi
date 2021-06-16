#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "iRoschool";       // Nama SSID AP/Hotspot
const char* password = "wlan3e1a3f";       // Password Wifi
  //#define durationSleep 1 //900 detik == 15menit // 60 detik == 1 menit cok

void setup() {
  Serial.begin(115200);
  delay(10);
  // Connect to WiFi network ------------------------------------------------
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
// Mengatur WiFi ----------------------------------------------------------
  WiFi.mode(WIFI_STA);                      // Mode Station
  WiFi.begin(ssid, password);               // Mencocokan SSID dan Password
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
// Print status Connect ---------------------------------------------------
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
//  Serial.setTimeout(2000);
//
//  // Wait for serial to initialize.
//  while(!Serial)
  Serial.println( "HELLO GUYS DAVID DISINI");
  delay(5000);
//  // Deep sleep mode for 30 seconds, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
//  Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");
 Serial.println("Aq Tidur dulu ea");
//  ESP.deepSleep(30e6); 
 ESP.deepSleep(10e6);// kali 1 juta / e6
  // Deep sleep mode until RESET pin is connected to a LOW signal (for example pushbutton or magnetic reed switch)
  //Serial.println("I'm awake, but I'm going into deep sleep mode until RESET pin is connected to a LOW signal");
  //ESP.deepSleep(0); 
}

void loop() {
}
