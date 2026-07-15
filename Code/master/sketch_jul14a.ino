
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

esp_now_peer_info_t slave;
uint8_t data = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // le baud

  WiFi.mode(WiFi_STA); // station mode
  esp_now_init();

  esp_now_register_send_cb(OnDataSent);
  ScanForSlave();



}

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
          Serial.print("coucou");
          Serial.println(BSSIDstr);
        } else {
          Serial.println("erreur d'ajout");
        }
      }
      break;
    }
  }

}


void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status){
  Serial.print("received...")
  Serial.println(data);
}

void loop() {

  esp_err_t result = esp_now_send(slave.peer_addr, (uint8_t*)&data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("sent");

  } else {
    Serial.println("error");
  }

  delay(3000);

}
