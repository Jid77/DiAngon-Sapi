#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
const char* ssid = "Daya Desa_IT";//"DiANGON"; //--> Your wifi name or SSID.
const char* password = "nalaktak2a";//"1234567890"; //--> Your wifi password.
const char* host = "script.google.com";
String google_ID = "AKfycbxLYsXe1aJwWWrHdP1GbslEpCwGezzMJ_s2gKWvBHjz8rKrvt90Xig-_SENE58-Cz0V"; 
WiFiClientSecure client; 

#include <SPI.h>
#include <SD.h>
File myFile;
unsigned SD_error = 1;

#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"
RTC_DS1307 rtc;
unsigned char error_rtc = 1;

#include <SimpleKalmanFilter.h>
SimpleKalmanFilter KalmanFilter_ax(14, 14, 0.02),
                   KalmanFilter_ay(14, 14, 0.02),
                   KalmanFilter_az(14, 14, 0.02),
                   KalmanFilter_gx(14, 14, 0.02),
                   KalmanFilter_gy(14, 14, 0.02),
                   KalmanFilter_gz(14, 14, 0.02);
const int MPU_ADDR = 0x69; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

#include <Adafruit_INA219.h>
Adafruit_INA219 sensor219;
unsigned char error_baterai = 1;
SimpleKalmanFilter KalmanFilter_ina219_arus(4, 4, 0.02);

#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 2;  
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
unsigned char error_suhu = 1;

String save_sd="";
unsigned char save_sd_count=0;
unsigned long deepsleep;

String batas_data_SDcard="11";
unsigned char batas_data_variabel = 7;
int set_tidur = 60*60, set_data = 30; //detik, detik

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  pinMode(2,1);  
  digitalWrite(2,0);
  String cek_sn = eeprom_read();
  if(cek_sn == "" || cek_sn.length()<3) Serial_Number();
  else if(cek_sn.substring(0,3) != "ISN") Serial_Number();
  digitalWrite(2,1);
  
  if(rtc.begin()){
    if(!rtc.isrunning()){
      Serial.println("\nRTC not running");
      error_rtc = 0;
    }
  }
  else{
    Serial.println("\nRTC error !!!");
    error_rtc = 0;
  }
  Wire.begin(); 
  
  if (!SD.begin(10)){
    Serial.println("SD card error !!!");
    SD_error = 0;
  }
  else{
    myFile = SD.open("History.txt", FILE_WRITE);
    myFile.close();
     if (!SD.exists("History.txt")){
        Serial.println("SD card error !!!");
        SD_error = 0;
     }
  }
  
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  if(!sensor219.begin()){  //baterai;
    Serial.println("sensor baterai error !!!");
    error_baterai = 0;
  }
  
  sensors.begin(); //suhu
 
  deepsleep = millis();
  Serial.println(eeprom_read());
  
}

// ======================= Main ==========================================
void loop() {
  String Read_serial = serial();
  if(Read_serial != ""){
    if(Read_serial == "*c"){
      eeprom_clear();
      Serial.println("eeprom clear");
    }
    Read_serial = "";
  }
  save_sd += read_sensor();
  if(save_sd_count >= batas_data_variabel){
    if(SD_error == 0){
      wifi();
      String datas = save_sd.substring(1);
      signed char tes_datas = send_data(datas);
      if(tes_datas==1){        
        Serial.println(datas);
        Serial.println("langsung kirim oke");
        Serial.println("tidur");
        ESP.deepSleep(1e6*set_tidur);
      }
      else{
        Serial.println(datas);
        Serial.println("langsung kirim gagal");
        Serial.println("tidur");
        ESP.deepSleep(1e6*set_tidur);
      }
    }
    if(sd_write(0, save_sd))eeprom_write(eeprom_set(3,"+1"));
    else Serial.println("\ngagal set sd card");
    Serial.println(eeprom_read());
    save_sd = "";
    save_sd_count = 0;
  }
  save_sd_count += 1;
  if(millis()-deepsleep >= 1000*set_data){
    wifi();
    unsigned char oke_send = 1;
    for(int x=eeprom_index(1).toInt(); x<=eeprom_index(2).toInt(); x++){
      String kirim_spreadsheet = sd_read(String(x)).substring(1);
      Serial.println("total = "+kirim_spreadsheet);
      if(!send_data(kirim_spreadsheet)){
        oke_send = 0;
        break;
      }
      else{
        sd_write(1, sd_read(String(x)));
        eeprom_set(1,"+1");
      }
    }
    if(oke_send==1){
      for(int y=eeprom_index(1).toInt(); y<=eeprom_index(2).toInt(); y++){
        bool cek_sd_card = sd_clear(String(y));
        if(cek_sd_card)Serial.println("remove data_"+String(y));
        else Serial.println("gagal remove data_"+String(y));
      }
      eeprom_write(eeprom_set(1,"0"));
    }
    Serial.println("tidur");
    ESP.deepSleep(1e6*set_tidur);
  }
}
// =======================================================================

String read_sensor(void){
  String date;
  if(error_rtc == 1){
    DateTime now = rtc.now();
     date= String(now.year())+"-"+
           String(now.month())+"-"+
           String(now.day())+"|"+
           String(now.hour())+"-"+
           String(now.minute())+"-"+
           String(now.second());
  }
  else date="?|?";
  String imu = imu6050();
  String ina;
  if(error_baterai == 1)ina = INA219();
  else ina = "?";
  String suhu_ds;
  if(error_suhu == 1)suhu_ds = ds18b20();
  else suhu_ds ="?";
  String V_mA;
  if(error_baterai == 1)V_mA = INA219_mA();
  else V_mA = "?|?";
  
  Serial.println("date : "+date);
  Serial.println("imu : "+imu);
  Serial.println("ina : "+ina);
  Serial.println("suhu_ds : "+suhu_ds);
  return ","+date+"|"+eeprom_index(0)+"|"+imu+"|"+ina+"|"+suhu_ds+"|"+V_mA;
}

void wifi(void){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  Serial.println();
  client.setInsecure();
}

signed char send_data(String data){
  if (!client.connect(host, 443)) return -1;
  String url = "/macros/s/" + google_ID + "/exec?data=" + data;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");
  unsigned long count_client=millis();
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") return 1;
    if (millis() - count_client >= 1000*30)return 0;
  }
  return 0;
}

String serial(void){
  String Data="";
  while (Serial.available() > 0) {
    char c = Serial.read();
    if(c != '\r' && c != '\n'){
      Data += c;
      delay(3);      
    }
  }
  return Data;
}

bool sd_write(unsigned char data_sd, String data){
  if(data_sd == 0){
    String cek_file = eeprom_index(2);
    if(cek_file == "")return 0;
    myFile = SD.open("data_"+String(cek_file)+".txt", FILE_WRITE);
  }
  else myFile = SD.open("History.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(data);
    myFile.close();
    return 1;
  }
  else{
    myFile.close();
    return 0;
  }
}

bool sd_clear(String text){
  String cek_text = "data_"+text+".txt";
  SD.remove(cek_text);
  if (SD.exists(cek_text))return 0;
  else return 1;
}

String sd_read(String number){
  myFile = SD.open("data_"+number+".txt");
  if (myFile) {
    String file = "";
    while (myFile.available()) {
      char Read = myFile.read();
      file += Read;
    }
    myFile.close();
    return file;
  } 
  else{
    myFile.close();
    return "0";
  }
}

void Serial_Number(void){  
  String data_serial, serial_number;
  while(1){
    data_serial = serial();
    if (data_serial != ""){
      if (data_serial.indexOf(",") != -1 && data_serial.indexOf("#") != -1){
        data_serial = data_serial.substring(data_serial.indexOf(",")+1,data_serial.indexOf("#"));
        if(data_serial.substring(0,3) == "ISN"){
          serial_number = data_serial.substring(0,data_serial.indexOf('-')); 
          if(RTC(data_serial)){
            Serial.println(","+serial_number+"#"); 
            eeprom_clear();
            eeprom_write(serial_number+"-1-1-0");            
            break;
          }
          else Serial.println("???");           
        }
      }
      data_serial = "";
    }
  }
  delay(1000*60);
}

String eeprom_index(int index){
  //ISN(0), setFile(1), file(2), count(3) 
  String arrayEread[4], parseData;
  String Eread = eeprom_read();
  for(unsigned char i=0; i<4; i++){
    if(i==0){
      arrayEread[i] = Eread.substring(0,Eread.indexOf("-"));
      parseData = Eread.substring(Eread.indexOf("-")+1);
    }
    else{
      arrayEread[i] = parseData.substring(0,parseData.indexOf("-"));
      if(i!=3)parseData = parseData.substring(parseData.indexOf("-")+1);
    }
  }
  return arrayEread[index];
}

String eeprom_set(int index, String setdata){
  //ISN(0), setFile(1), file(2), count(3) 
  String arrayEread[4], parseData, data;
  String Eread = eeprom_read();
  
  for(unsigned char i=0; i<4; i++){
    if(i==0){
      arrayEread[i] = Eread.substring(0,Eread.indexOf("-"));
      parseData = Eread.substring(Eread.indexOf("-")+1);
    }
    else{
      arrayEread[i] = parseData.substring(0,parseData.indexOf("-"));
      if(i!=3)parseData = parseData.substring(parseData.indexOf("-")+1);
    }
  }
  // ===================== ISN =====================
  if(index == 0)data= setdata+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
  // ===================== setFile =====================
  else if(index == 1){
    if(setdata == "+1") data= arrayEread[0]+"-"+String(arrayEread[1].toInt()+1)+"-"+arrayEread[2]+"-"+arrayEread[3];
    else if(setdata == "-1"){
      if(arrayEread[1] == "1")data= arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
      else data= arrayEread[0]+"-"+String(arrayEread[1].toInt()-1)+"-"+arrayEread[2]+"-"+arrayEread[3];
    }
    else if(setdata == "0") data= arrayEread[0]+"-1-1-0";
    else data = arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
  }
  // ===================== file =====================
  else if(index == 2){
    if(setdata == "+1") data= arrayEread[0]+"-"+arrayEread[1]+"-"+String(arrayEread[2].toInt()+1)+"-"+arrayEread[3];
    else if(setdata == "-1"){
      if(arrayEread[2] == "1")data= arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
      else data= arrayEread[0]+"-"+arrayEread[1]+"-"+String(arrayEread[2].toInt()-1)+"-"+arrayEread[3];
    }
    else if(setdata == "0") data= arrayEread[0]+"-1-1-0";
    else data = arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
  }
  // ===================== count =====================
  else if(index == 3){
    if(setdata == "+1"){
      if(arrayEread[3]==batas_data_SDcard) data= arrayEread[0]+"-"+arrayEread[1]+"-"+String(arrayEread[2].toInt()+1)+"-"+"1";
      else data= arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+String(arrayEread[3].toInt()+1);      
    }
    else if(setdata == "-1"){
      if(arrayEread[3]=="0"){
        if(arrayEread[2]=="1") data= arrayEread[0]+"-"+arrayEread[1]+"-"+"1"+"-"+"0";
        else data= arrayEread[0]+"-"+arrayEread[1]+"-"+String(arrayEread[2].toInt()-1)+"-"+batas_data_SDcard;
      }
      else data= arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+String(arrayEread[3].toInt()-1);
    }
    else if(setdata == "0") data= arrayEread[0]+"-1-1-0";
    else data = arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
  }
  else data = arrayEread[0]+"-"+arrayEread[1]+"-"+arrayEread[2]+"-"+arrayEread[3];
  return data;
}

void eeprom_write(String data)  {
  for (int addr = 0; addr < data.length(); addr++){
    EEPROM.write(addr, data[addr]);
  }
  EEPROM.write(data.length(), 0);
  EEPROM.commit(); 
}

String eeprom_read(void){
  int count=0;
  char baca[50];
  unsigned char c = EEPROM.read(count);
  while(c != 25 && count < 50){
    baca[count] = c;
    count++;
    c = EEPROM.read(count);
  }
  baca[count] = 0;
  return String(baca);
}

void eeprom_clear(void){
  for (int i = 0; i < 70; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

bool RTC(String data){
  const unsigned char arrayParse_count = 7;
  String arrayParse[arrayParse_count], parseData;
  for(unsigned char i=0; i<arrayParse_count; i++){
    if(i==0){
      arrayParse[i] = data.substring(0,data.indexOf("-"));
      parseData = data.substring(data.indexOf("-")+1);
    }
    else{
      arrayParse[i] = parseData.substring(0,parseData.indexOf("-"));
      if(i!=arrayParse_count-1)parseData = parseData.substring(parseData.indexOf("-")+1);
    }
  }
  if (rtc.begin()) {
    if (! rtc.isrunning()) {
      rtc.adjust(DateTime(arrayParse[1].toInt(), arrayParse[2].toInt(), arrayParse[3].toInt(), arrayParse[4].toInt(), arrayParse[5].toInt(), arrayParse[6].toInt()));
      DateTime now = rtc.now();
      String date = String(now.year())+
         String(now.month())+
         String(now.day())+
         String(now.hour());
//      Serial.println("mati -> "+arrayParse[1]+arrayParse[2]+arrayParse[3]+arrayParse[4]+" <--> "+date);
      if(arrayParse[1]+arrayParse[2]+arrayParse[3]+arrayParse[4] == date)return 1;
    }
    else{
      rtc.adjust(DateTime(arrayParse[1].toInt(), arrayParse[2].toInt(), arrayParse[3].toInt(), arrayParse[4].toInt(), arrayParse[5].toInt(), arrayParse[6].toInt()));
      DateTime now = rtc.now();
      String date = String(now.year())+
         String(now.month())+
         String(now.day())+
         String(now.hour());
//      Serial.println("hidup -> "+arrayParse[1]+arrayParse[2]+arrayParse[3]+arrayParse[4]+" <--> "+date);
      if(arrayParse[1]+arrayParse[2]+arrayParse[3]+arrayParse[4] == date)return 1;
    }
    return 0;
  }
  else return 0;
}

String imu6050(void){
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  
  accelerometer_x = int((KalmanFilter_ax.updateEstimate(accelerometer_x)/100) *0.552-6.074);
  accelerometer_y = int((KalmanFilter_ay.updateEstimate(accelerometer_y)/100) *0.554-0.831);
  accelerometer_z = int((KalmanFilter_az.updateEstimate(accelerometer_z)/100) *0.545-0.545);
  gyro_x = int(KalmanFilter_gx.updateEstimate(gyro_x)/100);
  gyro_y = int(KalmanFilter_gy.updateEstimate(gyro_y)/100);
  gyro_z = int(KalmanFilter_gz.updateEstimate(gyro_z)/100);

  if(accelerometer_x > 90){accelerometer_x = 90;}
  if(accelerometer_x < -90){accelerometer_x = -90;}
  if(accelerometer_y > 90){accelerometer_y = 90;}
  if(accelerometer_y < -90){accelerometer_y = -90;}
  if(accelerometer_z > 90){accelerometer_z = 90;}
  if(accelerometer_z < -90){accelerometer_z = -90;}
  
  return String(accelerometer_x)+"|"+
         String(accelerometer_y)+"|"+
         String(accelerometer_z)+"|"+
         String(gyro_x)+"|"+
         String(gyro_y)+"|"+
         String(gyro_z);
}

String INA219(void){
  int Tegangan;
  Tegangan = round(100.0*(sensor219.getBusVoltage_V()-3.3)/0.7);
  if(Tegangan > 100){Tegangan = 100;}
  if(Tegangan < 0){Tegangan = 0;}
  return String(Tegangan);
}

String ds18b20(void){
  float suhu, suhu_1;
  String suhu_objek;
  int count=0;
  sensors.requestTemperatures(); 
  suhu_1 = sensors.getTempCByIndex(0);
  suhu_objek = String(suhu_1,1);
  delay(200);
  while(1){
    sensors.requestTemperatures(); 
    suhu = sensors.getTempCByIndex(0);
    delay(200);
    if(abs(suhu - suhu_1) <= 0.1)
      count += 1;
    else{
      count = 0;
      suhu_objek = String(suhu_1,1);
    }
    suhu_1 = suhu;
    if(suhu < 0)
      return "0";
    if(suhu > 70)
      return "0";
    if(count >= 2)
      return suhu_objek;
  }
}

String INA219_mA(void){
  float Tegangan, Arus;
  Tegangan = sensor219.getBusVoltage_V();
  Arus = sensor219.getCurrent_mA();
  if (Arus < 0)return "0.00";
  else{
    Arus = KalmanFilter_ina219_arus.updateEstimate(Arus)+0.93;
  }
  return String(Tegangan, 2)+"|"+String(Arus, 2);
}
