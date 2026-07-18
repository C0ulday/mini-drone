
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

esp_now_peer_info_t slave;

// message structure
typedef struct {
  char deviceName[32];
  uint8_t motorSpeed;
  bool arm;
  unsigned long timestamp;
} Message;

Message incomingMessage;
uint8_t broadAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// if master sent the message
void OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status){
  Serial.print("sent...");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

// setting up the master
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // baud rate
  WiFi.mode(WIFI_STA); // station mode
  strcpy(incomingMessage.deviceName,"Drone master");
  
  if (esp_now_init() != ESP_OK){
    Serial.println("error init ESP_NOW");
    return;
  } 

  esp_now_register_send_cb(OnDataSent);
  memcpy(slave.peer_addr, broadAddr, 6);
  slave.channel = CHANNEL;
  slave.encrypt = 0;

  if(esp_now_add_peer(&slave)== ESP_OK) {
    Serial.println("peer added...");
  } else {
    Serial.println("peer not added...");
    return;
  }

  
}



void loop() {
  incomingMessage.timestamp = millis();
  incomingMessage.motorSpeed = 128;
  incomingMessage.arm = true;
  
  esp_err_t result = esp_now_send(slave.peer_addr, (uint8_t*)&incomingMessage, sizeof(incomingMessage));

  if (result == ESP_OK) {
    Serial.println("sent");

  } else {
    Serial.println("error");
  }

  delay(3000);

}
