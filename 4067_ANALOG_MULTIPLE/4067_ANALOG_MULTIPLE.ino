//en GND
//  ANALOG MULTIPLE SENSOR
//  BY IROStech
//  JID
int s0 =12;//d6;
int s1 =14;//d5;
int s2 =2;//d4;
int s3 =0;//d3;
/////
int sig = A0;
int Suhu= 0;
    float EMA_a = 0.6;      //initialization of EMA alpha
    int EMA_S = 0;          //initialization of EMA S
void setup() {
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 
  Serial.begin(9600);
  pinMode(sig, INPUT);
  EMA_S = analogRead(sig);  //set EMA S for t=1
}
//S2,S1,S0
void changeMux(int a, int b, int c, int d) {
  digitalWrite(s0, a);
  digitalWrite(s1, b);
  digitalWrite(s2, c);
  digitalWrite(s3, d);
}
void loop() {
  

float Rotary_encoder;
float IMU;
 //C6 BMS??
  changeMux(LOW, HIGH, HIGH, LOW);
  Suhu = analogRead(sig); //Value of the sensor connected Option 3 pin of Mux
  EMA_S = (EMA_a*Suhu) + ((1-EMA_a)*EMA_S);
  Serial.print ("P_1 ");
  Serial.println(EMA_S);
  delay(1000);
  
// //C7 IMU??
//  changeMux(HIGH, HIGH, HIGH, LOW);
//  Rotary_encoder = analogRead(sig); //Value of the sensor connected Option 3 pin of Mux
//  Serial.print ("P_2 ");
//  Serial.println(value2);
//  delay(1000);
// //C8
//  changeMux(LOW, LOW, LOW, HIGH);
//  IMU = analogRead(sig); //Value of the sensor connected Option 3 pin of Mux
//  Serial.print ("ACS ");
//  Serial.println(value2);
//  delay(1000);
}
