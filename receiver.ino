#include <esp_now.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void opening();
void perintah();
void mainscreen();
uint8_t broadcastAddress[] = {0xC8, 0x2E, 0x18, 0x8D, 0xF1, 0x84}; //5C 01 3B 6A 27 64 // 5C 01 3B 70 2B 54
LiquidCrystal_I2C lcd(0x26,20,4);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct message_transmitter 
{
  int up;
  int down;
  int left;
  int right;
  int square;
  int circle;
}message_transmitter;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct message_receiver
{
  int dist;
  int servoPos1_1;
  int servoPos2_1;
  int motor1_1;
  int motor2_1;
  char direction[10];
}message_receiver;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
message_receiver receiverData;
message_transmitter transmitterData;
esp_now_peer_info_t peerInfo;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void opening ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RC robot sampah");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mainscreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("tombol: ");
  lcd.setCursor(0,1);
  lcd.print("jarak objek: ");
  lcd.print(receiverData.dist);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void perintah()
{
  if (transmitterData.up ==0)
  {
    lcd.clear();
    mainscreen();
    lcd.setCursor(7,0);
    lcd.print("maju");
    delay(500);
    mainscreen();
  }
  else if (transmitterData.down ==0)
  {
    lcd.clear();
    mainscreen();
    lcd.setCursor(7,0);
    lcd.print("mundur");
    delay(500);
    mainscreen();
  }
 else if (transmitterData.left ==0)
  {
    lcd.clear();
    mainscreen();
    lcd.setCursor(7,0);
    lcd.print("kiri");
    delay(500);
    mainscreen();
  }
  else if (transmitterData.right ==0)
  {
    lcd.clear();
    mainscreen();
    lcd.setCursor(7,0);
    lcd.print("kanan");
    delay(500);
    mainscreen();
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  //Serial.println(receiverData.dist);
  //Serial.println(receiverData.servoPos_1);
  Serial.println(receiverData.direction);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(115200);
  lcd.init();               
  lcd.backlight();
  opening();
  delay(5000);
  mainscreen();
  pinMode(15, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  pinMode(23, INPUT);
  WiFi.mode(WIFI_STA);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void loop() 
{
  transmitterData.up = digitalRead(15);
  transmitterData.down = digitalRead(17);
  transmitterData.left = digitalRead(16);
  transmitterData.right = digitalRead(18);
  transmitterData.square = digitalRead(19);
  transmitterData.circle = digitalRead(23); 
  perintah();
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &transmitterData, sizeof(transmitterData));
  delay(50);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
