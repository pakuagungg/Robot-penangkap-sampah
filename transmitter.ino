#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Servo myservo1;
Servo myservo2;
int pos = 0;
int servoPin1 = 13;
int servoPin2 = 12;
int ENA = 22;
int IN1 = 19;
int IN2 = 18;
int ENB = 23;
int IN3 = 16;
int IN4 = 17;
int a = 255;//speedmotor
int b = 20;//delaymotor
int duration, distance;
const int trigPin = 2;
const int echoPin = 4;
uint8_t broadcastAddress[] = {0x5C, 0x01, 0x3B, 0x70, 0x2B, 0x54};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct message_transmitter
{
  int up;
  int down;
  int left;
  int right;
  int square;
  int circle;
  int dist;
}message_transmitter;
///////////////////////////////////////////////////////////////////////////////////////////
typedef struct message_receiver
{
  int dist;
  int servoPos1_1;
  int servoPos2_1;
  int motor1_1;
  int motor2_1;
  char direction[10];
}message_receiver;
/////////////////////////////////////////////////////////////////////////////////////////
message_receiver receiverData;
message_transmitter transmitterData;
esp_now_peer_info_t peerInfo;
/////////////////////////////////////////////////////////////////////////////////////
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//////////////////////////////////////////////////////////////////////////////////
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  memcpy(&transmitterData, incomingData, sizeof(transmitterData));
  Serial.print(transmitterData.up);
  Serial.print(transmitterData.down);
  Serial.print(transmitterData.left);
  Serial.print(transmitterData.right);
  Serial.print(transmitterData.square);
  Serial.println(transmitterData.circle);
}
///////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(115200);
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo1.setPeriodHertz(50);    
  myservo2.setPeriodHertz(50);
	myservo1.attach(servoPin1, 1000, 2000);
  myservo2.attach(servoPin2, 1000, 2000);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  WiFi.mode(WIFI_STA);
	pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  
///////////////////////////////////////////////////////////////
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
///////////////////////////////////////////////////////////////////////////    
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}
//////////////////////////////////////////////////////////////////////////////
void loop() 
{
/////////////////////////////////////////////////   HCSR MODULE   ///////////////////////////////////////////////////////////////////
   digitalWrite(trigPin, LOW);  
	 delayMicroseconds(2);  
	 digitalWrite(trigPin, HIGH);  
	 delayMicroseconds(10); 
   digitalWrite(trigPin, LOW);   
   duration = pulseIn(echoPin, HIGH); 
   distance = (duration*0.0344)/2;
   Serial.print("jarak");
   Serial.print(distance);
   Serial.println("cm");
   receiverData.dist = distance;
////////////////////////////////////////////////   SERVO MOVEMENT   ///////////////////////////////////////////////////////////////////
  if(transmitterData.square == 0)
  {
    myservo1.write(pos); 
    myservo2.write(pos);
    pos++;
  }
  else if(transmitterData.circle == 0)
  {
    myservo1.write(pos);
    myservo2.write(pos);
    pos--;
  }
  if(pos > 180) pos = 180;
  if(pos < 0) pos = 0;
  receiverData.servoPos1_1 = pos;
  receiverData.servoPos2_1 = pos;
//////////////////////////////////////////////   MOTOR MOVEMENT   ////////////////////////////////////////////////////////////////////  
  if(transmitterData.up == 0)
 {
    delay(b);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, a);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, a);
    delay(b);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
    else if(transmitterData.down == 0)
  {
    delay(b);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, a);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, a);
    delay(b);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
   else if(transmitterData.right == 0)
  {
  
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, a);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, a);
    delay(b);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
   else if(transmitterData.left == 0)
  {
  
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, a);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, a);
    delay(b);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
  receiverData.motor1_1 = ENA;
  receiverData.motor2_1 = ENB;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &receiverData, sizeof(receiverData));
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  delay(20);
}
