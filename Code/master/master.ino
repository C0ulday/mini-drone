
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

// if master sent the message
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status){
  Serial.print("sent...");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

// setting up the master
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // baud rate

  WiFi.mode(WIFI_STA); // station mode
  
  if (esp_now_init() != ESP_OK){
    Serial.println("error init ESP_NOW");
    return;
  } 

  esp_now_register_send_cb(OnDataSent);
  ScanForSlave();
}

// looking for slaves
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();

  for (int i =0 ; i < scanResults ; ++i) {
    String SSID = WiFi.SSID(i);
    String BSSIDstr = WiFi.BSSIDstr(i);

    
    if (SSID.indexOf("RX") ==0) {

      int mac[6];

      if(6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1],&mac[2],&mac[3],&mac[4],&mac[5])) {
        
        for (int j =0; j <6 ; j++) {
          slave.peer_addr[j]  = (uint8_t)mac[j];
        }
    
        slave.channel = CHANNEL;
        slave.encrypt = 0; // no encryption
        if(esp_now_add_peer(&slave) == ESP_OK) {
          Serial.print("connected...");
          Serial.println(BSSIDstr);
        } else {
          Serial.println("cannot connect");
        }
      }
      break;
    }
  }

}


void loop() {

  strcpy(incomingMessage.deviceName,"Drone master");
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
