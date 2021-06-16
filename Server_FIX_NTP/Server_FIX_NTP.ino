#include <ArduinoJson.h>
//#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

//Library Komunikasi Serial NodeMCU to Arduino
#include <SoftwareSerial.h>

#include <SD.h>

//Data
String dataIn;
String dt[100];//data hasil parsing
int i;
boolean parsing = false;
String req;
// firebase
#define FIREBASE_HOST "fir-76e3d-default-rtdb.firebaseio.com" //please remove http and / from firebase url
#define FIREBASE_AUTH "jnMxHSMSIzm2AfclbvrYjJ3vZWhAVfMzowa9Ma9n" // paste secret key here

///Membuat Hotspot untuk Client dan konek WIFI
const char* ssid_AP = "ANGON WiFi";      // Nama AP/Hotspot
const char* password_AP = "qwe12345";    // Password AP/Hotspot

// Replace with your network credentials
//const char *ssid     = "JID";
//const char *password = "haq979tx77";

//modem
//const char* ssid = "DiANGON";
//const char* password = "1234567890";

//iROStech
const char* ssid = "iRoschool";
const char* password = "wlan3e1a3f";

//modem
//const char* ssid = "Mas Jawa";
//const char* password = "apayaapayaapaya";
//#define CS_PIN D8; //d8 SD card

              //Komunikasi Serial
              //SoftwareSerial mySerial(,4);//rx D1 ,tx D2
              
IPAddress ip(192,168,4,1); // arbitrary IP address (doesn't conflict w/ local network)
IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);
int n =4; //random(0, 1000);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

//Month names
String months[12]={"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
String currentDate;
String formattedTime;
String waktu;
void setup() {
  
  Serial.begin(115200);
 // Serial1.begin(115200);
  delay(10);
  dataIn="";
  
  // Mengatur WiFi ----------------------------------------------------------
  Serial.println();
  Serial.print("Configuring access point...");

  WiFi.mode(WIFI_AP_STA);             // Mode AP/Hotspot
  WiFi.softAPConfig(ip,gateway,subnet);
  WiFi.softAP(ssid_AP, password_AP);
  WiFi.begin(ssid, password);
  
  // Konek Ke WiFi
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
   }
   Serial.println("");
   Serial.println("WiFi Connected to ");
   Serial.print(ssid);
   Serial.print("");
  // Start the server -------------------------------------------------------
  server.begin();
  Serial.println("Server dijalankan");

  // Print the IP address ---------------------------------------------------
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(WiFi.localIP());
  Serial.println(myIP);
  
  //Konek ke Firebase
 // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
// Initialize a NTPClient to get time
//25200 Indonesia
  timeClient.begin();
  timeClient.setTimeOffset(25200);

  //CEK Apakah Kartu SD tersedia atau tidak!
  Serial.println(" Inisialisasi SD card....");
  SD.begin(15);
  if (!SD.begin(15)) {
    Serial.println("Gagal, cek apakah kartu SD tersedia..");
    return;
  }
  Serial.println("Kartu OK");
}
//

void loop() {
  //Void Waktu
//  ntp();

  
  // Cek client jika telah tersambung
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  //Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  
  //  Membaca Data dari Client Lalu mencoba Memparsingnya
  if(client.available()>0){
  req = client.readStringUntil('\n');//('\r');
  Serial.println(req);
  dataIn += req;
  if(req.indexOf("IMU ")){
    parsing = true;
    Serial.println("ON");
  }
//  }
//  
////------------------ PARSING AND SEND DATA CLIENT TO FIREBASE  
//  if(parsing){
//    parsingData();
//    
//    //Kirim Data to firebase
//    //dt[0]= ID Client    dt [1] = IMU
////Serial Number--------------------------------------------------
//Firebase.setString(dt[0] + "/SN"
//                  ,dt[0]);
////IMU ACCEL------------------------------------------------------
//Firebase.setString(dt[0] + "/IMU" + "/Accel" + "/X"
//                  ,dt[3]);
//Firebase.setString(dt[0] + "/IMU"  + "/Accel" + "/Y"
//                  ,dt[4]);
//Firebase.setString(dt[0] + "/IMU"  + "/Accel" + "/Z"
//                  ,dt[5]);
////IMU GYRO-------------------------------------------------------                 
//Firebase.setString(dt[0] + "/IMU"  + "/Gyro" + "/X"
//                  ,dt[7]);
//Firebase.setString(dt[0] + "/IMU"  + "/Gyro" + "/Y"
//                  ,dt[8]);
//Firebase.setString(dt[0] + "/IMU"  + "/Gyro" + "/Z"
//                  ,dt[9]);
//
//                  
////BMS-------------------------------------------------------------
////Firebase.setString("Client/ " + dt[0] + "/Sensor" + " Battery Management System" + "/SOH"
////                  ,"Gk tau");
//Firebase.setString(dt[0] + "/Suhu Lingkungan"
//                    ,dt[10] + "*");
//Firebase.setString(dt[0] + "/Lingkar DADA"
//                    ,dt[11] + "cm");
//Firebase.setString(dt[0] + "/BMS" + "/Soc"
//                    ,dt[13] + "%");
////JAM-------------------------------------------------------------
//Firebase.setString(dt[0] + "/TIME" + "/Tanggal"
//                    ,currentDate + "%");
//Firebase.setString(dt[0] + "/TIME" + "/Jam"
//                    ,formattedTime + "%");
//////Rotary Encoder
////Firebase.setString("Client/ " + dt[0] + "/Sensor" + " Rotary"
////                  ," CM");
////    
//
//              
//    Serial.println("DATA FIREBASE UPLOADED");
//    parsing=false;
//    dataIn="";
//  }  
              /*
              float n = (req.indexOf("I
              //Serial.println(req.indexOf());
              if(req.indexOf(("ID " + String(n)))){
              Firebase.setString("Client/SAPI_" + String(n) ,req);
              Serial.println("ID updated");
              delay(1000);
              }
               client.flush();
              Serial.println("Client disonnected");
              */
 
  delay(10);
  
    //Menyimpan Data pada SD card
  if(SD.begin(15)){
  File dataFile = SD.open("DATA_ANGON.txt", FILE_WRITE);
  if (dataFile) {
    Serial.println("Menyimpan data...");
    dataFile.println(req);
    dataFile.close();
    Serial.println("Data telah disimpan " + req);
  }
    else {
    Serial.println("tidak bisa disimpan");
  }
  }
       /* 
        Serial1.println(req);
        delay(1000);
        
        Send data to Firebase
        Firebase.setString("Sensor_Client/Value",req);
        delay(1000);
       */

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
}
//Void PARSING DATA DARI CLIENT----------------------------------------
void parsingData(){

int j=0;
 
//kirim data yang telah diterima sebelumnya
Serial.print("data masuk : ");
Serial.print(dataIn);
Serial.print("\n\n");
 
//inisialisasi variabel, (reset isi variabel)
dt[j]="";

//proses parsing data
for(i=1;i<dataIn.length();i++){

//pengecekan tiap karakter dengan karakter (#) dan (,)
//# Penutup Data
//, Pemisah Data
if ((dataIn[i] == ']') || (dataIn[i] == ','))
{

//increment variabel j, 
//digunakan untuk merubah index array penampung
j++;
dt[j]="";       //inisialisasi variabel array dt[j]

}
else
{
//proses tampung data saat pengecekan karakter selesai.
dt[j] = dt[j] + dataIn[i];
}
}
 
//kirim data hasil parsing
//Hasil data berupa String bisa ubah ke data lain 
//Seperti toInt , toFloat , toDouble dll
//dt[]  mulai dari nol 0

Serial.print("ID : ");
Serial.print(dt[0]);
Serial.print("\n");

Serial.print("SensorType : ");// IMU
Serial.print(dt[1]);
Serial.print("\n");

////////ACCEL
Serial.print("IMU : ");
Serial.print(dt[2]); //accel
Serial.print("\n");
Serial.print("X : ");
Serial.print(dt[3].toDouble());//x
Serial.print("\n");
Serial.print("Y : ");
Serial.print(dt[4].toDouble());//y
Serial.print("\n");
Serial.print("Z : ");
Serial.print(dt[5].toDouble());//z
Serial.print("\n");
//Serial.print("Sqrt : ");
//Serial.print(dt[6].toDouble());//Sqrt
//Serial.print("\n");


Serial.print("IMU : ");
Serial.print(dt[6]);  //GYRO
Serial.print("\n");
Serial.print("X : ");
Serial.print(dt[7].toDouble());//X
Serial.print("\n");
Serial.print("Y : ");
Serial.print(dt[8].toDouble());//Y
Serial.print("\n");
Serial.print("Z : ");
Serial.print(dt[9].toDouble());//Z
Serial.print("\n");

Serial.print("SensorType : SUHU  ");// BMS
Serial.print("\n");
Serial.print(dt[10]);
Serial.print("\n");
Serial.print("SensorType : Sabuk ");
Serial.print("\n");
Serial.print(dt[11].toDouble());//X
Serial.print("\n");
Serial.print("SensorType : ");
Serial.print(dt[12].toDouble());//Y
Serial.print("\n");
Serial.print(dt[13].toDouble());//Y
Serial.print("\n");
//  Serial.print("Z : ");
//Serial.print(dt[14].toDouble());//Z
//Serial.print("\n");
}

//VOID NTPCLIENT--------------------------------------------------------
void ntp(){
   timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
 // Serial.print("Epoch Time: ");
 // Serial.println(epochTime);
  
  formattedTime = timeClient.getFormattedTime();
 // Serial.print("Formatted Time: ");
 // Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
 // Serial.print("Hour: ");
 // Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
 // Serial.print("Minutes: ");
 // Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
 // Serial.print("Seconds: ");
 // Serial.println(currentSecond);  

  String weekDay = weekDays[timeClient.getDay()];
 // Serial.print("Week Day: ");
 // Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
 // Serial.print("Month day: ");
 // Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
 // Serial.print("Month: ");
 // Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
 // Serial.print("Month name: ");
 // Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
 // Serial.print("Year: ");
 // Serial.println(currentYear);

  //Print complete date:
  currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
 // Serial.print("Current date: ");
//  Serial.println(currentDate);

waktu = currentDate + " " + formattedTime;

 // Serial.println(waktu);
  Serial.println("");
  delay(100);
}
