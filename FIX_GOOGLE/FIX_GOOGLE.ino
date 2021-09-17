#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_INA219.h>
#include "RTClib.h"

//#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
#define ONE_WIRE_BUS 2

//const char* ssid = "Daya Desa_IT"; //--> Your wifi name or SSID.
//const char* password = "nalaktak2a"; //--> Your wifi password.
const char* ssid = "Diangon-Modem"; //--> Your wifi name or SSID.
const char* password = "1234567890"; //--> Your wifi password.
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client; //--> Create a WiFiClientSecure object.
String GAS_ID = "AKfycbwY69X3SiNY4uwHq1kog4vUB-KLxm-k1vE8AAh6WsgEQXb583mqlYKtPddnfQ7ibfqJ";


//============================================================================== void setup

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float tempC = 0;

//---------------------------------------INA
Adafruit_INA219 ina219;
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  float soc = 0;
  int persen = 0;
//---------------------------------------INA

//---------------------------------------IMU
#define MPU6050_ADDR              0x69 // Alternatively set AD0 to HIGH  --> Address = 0x69
#define MPU6050_GYRO_CONFIG       0x1B ///< Gyro specfic configuration register
#define MPU6050_ACCEL_CONFIG      0x1C 
#define MPU6050_ACCEL_XOUT_H      0x3B
#define MPU6050_PWR_MGT_1         0x6B
#define MPU6050_SLEEP             0x06
typedef enum {
  MPU6050_ACC_RANGE_2G,  // +/- 2g (default)
  MPU6050_ACC_RANGE_4G,  // +/- 4g
  MPU6050_ACC_RANGE_8G,  // +/- 8g
  MPU6050_ACC_RANGE_16G // +/- 16g
} mpu6050_acc_range;
typedef enum {
  MPU6050_GYR_RANGE_250,  // +/- 250 deg/s (default)
  MPU6050_GYR_RANGE_500,  // +/- 500 deg/s
  MPU6050_GYR_RANGE_1000, // +/- 1000 deg/s
  MPU6050_GYR_RANGE_2000  // +/- 2000 deg/s
} mpu6050_gyr_range;
int16_t accX, accY, accZ, gyroX, gyroY, gyroZ, tRaw; // Raw register values (accelaration, gyroscope, temperature)
char result[7];
//---------------------------------------IMU

  
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
String tgl;
String data_sensor;


void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.println("");
  delay(5000);
  //RTC-----------------------
  if (! rtc.begin()) {
    Serial.println("Couldn't filnd RTC");
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC witrdeqh an explicit date & time, for example to set// January 21, 2014 at 3am you would call:// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  //SUHU----------------------
  sensors.begin();
  
  //INA219--------------------
  uint32_t currentFrequency; 
    if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    //while (1) { delay(10); }
  }
  
  //IMU-----------------------
  Wire.begin();
  MPU6050_wakeUp();
  setAccRange(MPU6050_ACC_RANGE_16G);
  setGyrRange(MPU6050_GYR_RANGE_250);

  int h = random(10);
  //PROGRAM==============================================\\ 
  delay(500);
  ina(); 
  imuu();
  jam();
  suhu();
  wifi();
  delay(500);
  tampung_data();
  sendData(String (data_sensor), h); //--> Calls the sendData Subroutine
  ESP.deepSleep(1800e6);//30menit
}




void loop() {  

  }




//================================SENSOR PROGRAM
void tampung_data(void){
                                      //  int h = random(10);
                                      //  // Read temperature as Celsius (the default)
                                      //  int t = random(10,101);
                                      //  String Temp = "Temperature : " + String(t) + " °C";
                                      //  String Humi = "Humidity : " + String(h) + " %";
                                      //  Serial.println(Temp);
                                      //  Serial.println(Humi);

//  String SN = "ISN1012068";//HITAM
//  String SN = "ISN1948009";//abu
    String SN = "ISN1326967";//hijau muda
//  String WAKTU= "Waktu";
//  String IMU= "1";
//  String SUHU = "Suhu";
//  String BATERAI= "Baterai"; 

  String WAKTU= String(tgl);
  String IMU= String(accX)+"|"+String(accY)+"|"+String(accZ)+"|"+String(gyroX)+"|"+String(gyroY)+"|"+String(gyroZ);
  String SUHU = String(tempC);
  String BATERAI= String(persen)+"|"+String(loadvoltage)+"|"+String(shuntvoltage); 
  
  data_sensor = String(SN)+"|"+ String(WAKTU)+"|"+ String(SUHU)+"|"+ String(BATERAI)+"|"+ String(IMU);  
 

}
void sendData(String tem,int hum) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature =  String(tem);
  // String string_temperature =  String(tem, DEC); 
  String string_humidity =  String(hum, DEC); 
  String url = "/macros/s/" + GAS_ID + "/exec?data=" + tem; //temperature=" + string_temperature + "&humidity=" + string_humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
} 




//================================SENSOR PROGRAM

  //DS1307--------------------------------------
void jam(void){
  DateTime now = rtc.now();
         tgl = String(now.year())+"/"+
               String(now.month())+"/"+
               String(now.day())+"/"+"("+
               String(daysOfTheWeek[now.dayOfTheWeek()])+")"+
               String(now.hour())+":"+
               String(now.minute())+":"+
               String(now.second());
  Serial.println(tgl);
  
//  Serial.print(now.year(), DEC);
//  Serial.print('/');
//  Serial.print(now.month(), DEC);
//  Serial.print('/');
//  Serial.print(now.day(), DEC);
//  Serial.print(" (");
//  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
//  Serial.print(") ");
//  Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.print(now.second(), DEC);
  Serial.println("");
  // calculate a date which is 7 days, 12 hours, 30 minutes, and 6 seconds into the future
  DateTime future (now + TimeSpan(7,12,30,6));
  delay(2000);
    
}

//SUHU====================================================
void suhu(void){
  sensors.requestTemperatures(); // Send the command to get temperatures
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature: ");
    Serial.println(tempC);
  } 
  else
  {
    Serial.println("Could not read temperature data!!!!");
  }
}

//INA======================================================
void ina(void){
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  Serial.print("Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:  "); Serial.print(current_mA); Serial.println(" mA");
         //3.3 = minimal Volt
       //0.7 = Max volt - Min volt
  persen = (loadvoltage - 3.3)/0.7*100;
  // persen = (3.8 - 3.3)/0.7*100;
  if(persen > 100){persen = 100;}
  if(persen < 0){persen = 0;}
  //return String(persen);
  delay(2000);
}





//IMU====================================================
void imuu(void){
  delay(500);
  MPU6050_wakeUp();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_ACCEL_XOUT_H); // starting with register ACCEL_XOUT_H
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. 
                               // As a result, the connection is kept active.
  Wire.requestFrom(MPU6050_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accX = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accY = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accZ = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  tRaw = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyroX = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyroY = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyroZ = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  MPU6050_sleep();
  
  // print out data
  Serial.print("AcX = "); Serial.print(toStr(accX));
  Serial.print(" | AcY = "); Serial.print(toStr(accY));
  Serial.print(" | AcZ = "); Serial.print(toStr(accZ));
  Serial.print(" | GyX = "); Serial.print(toStr(gyroX));
  Serial.print(" | GyY = "); Serial.print(toStr(gyroY));
  Serial.print(" | GyZ = "); Serial.print(toStr(gyroZ));
  Serial.println();
  delay(1000);
}

char* toStr(int16_t i) { // int16 to string plus output format
  sprintf(result, "%6d", i);
  return result;
}
void setAccRange(mpu6050_acc_range range){
  writeRegister(MPU6050_ACCEL_CONFIG, range<<3);
}
void setGyrRange(mpu6050_gyr_range range){
  writeRegister(MPU6050_GYRO_CONFIG, range<<3);
}
void MPU6050_wakeUp(){
  writeRegister(MPU6050_PWR_MGT_1, 0); 
  delay(30); // give him time to wake up, gyro needs quite a while to stabilize;
}
void MPU6050_sleep(){
  writeRegister(MPU6050_PWR_MGT_1, 1<<MPU6050_SLEEP); 
}
void writeRegister(uint16_t reg, byte value){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg); 
  Wire.write(value); 
  Wire.endTransmission(true);
}
//IMU====================================================



void wifi(){
    
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
  digitalWrite(LED_BUILTIN, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(LED_BUILTIN, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}



//==============================================================================
