#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_INA219.h> 
#include <MPU6050.h>
#include <EEPROM.h>
//const int LED =15; //D8
const int LED =5; //D
//SERIALL NUMBER-------------------------------------------------
  String Data;
  void EEPROM_put(char add, String data);
  String EEPROM_get(char add);
  String sn;
  String saveddata;
  int x=0;
  String Serial_number;
  
  String dataIn;
  String dt[100];
  int i;
  
void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(LED, OUTPUT);
  dataIn="";
  delay(50);
  // INPUT SERIAL NUMBER---------------------------------------------
  // Jika Serial number tidak ada / Tidak Sesuai (IRS...)
  if(sn !="IRS"){
  // Input Data Dari Serial Monitor
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    dataIn += c;
      if (c == '\n'){
                      parsingData();
                      Data = dt[1];
                      delay(100);
                      dataIn="";//mengosongkan
                      }
            //Data += c;
           // Serial.println(Data);  
      }

  
  if (Data.length() > 0) {
    Serial.print("Berhasil Menyimpan Data: ");
    //Serial.println(Data);
    EEPROM_put(5, Data);//9 Alamat EEPROM
    Data = "";
  }
  
  //Saved data dari UART
  saveddata = EEPROM_get(5);//9 Alamat EEPROM
  Serial.print ("Serial Number : ");
  Serial.println(saveddata);
  Serial.println("");
  delay(100);
//----------------------------------------------------------------------- 

// Cek Serial number memiliki CODE " IRS "(3 huruf pertama) 
  sn = saveddata.substring(0,3);
  //Serial.print("Kode unik : ");
  Serial.println(sn);
  Serial.println("Kode unik iROStech : IRS");
  Serial.println(saveddata);
  //Hidupkan program client 
  //Serial,Number
  Serial_number = saveddata.substring(0,10);
  delay(1000);
  }
  
  //jika Serial number memiliki kode unik
   if(sn == "IRS"){
    digitalWrite(LED, HIGH);
   
   //Program Client
   
   }
}


void loop() {
   }


//PARSING DATA------------------------------------------------------------------------------
void parsingData()
{
      int j=0;
      //kirim data yang telah diterima sebelumnya
      Serial.print("Data masuk : ");
      //Serial.print(dataIn);
   //   Serial.print("\n");
      
      //inisialisasi variabel, (reset isi variabel)
      dt[j]="";
      delay(50);
      //proses parsing data
      for(i=1;i<dataIn.length();i++)
      {
             //pengecekan tiap karakter dengan karakter (#) dan (,)
             if ((dataIn[i] == '#') || (dataIn[i] == ','))
             {
                   //increment variabel j, digunakan untuk merubah index array penampung
                   j++;
                  dt[j]=""; //inisialisasi variabel array dt[j]
             }
             else
             {
                  //proses tampung data saat pengecekan karakter selesai.
                  dt[j] = dt[j] + dataIn[i];
             }
      }
      //kirim data hasil parsing
    //  Serial.print("Data 1 : ");
  //    Serial.print(dt[0]);
//      Serial.print("\n");
//Serial.print("Data masuk : ");
Serial.println(dt[1]);
}
//PARSING DATA------------------------------------------------------------------------------

//VOID SAVE SN KE EEPROM--------------------------------------------------------
void EEPROM_put(char add, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
}
String EEPROM_get(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

//VOID SAVE SN KE EEPROM END--------------------------------------------------------
