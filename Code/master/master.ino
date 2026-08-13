
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
#define THROTTLE_PIN 1
#define YAW_PIN 2
#define ROLL_PIN 3
#define PITCH_PIN 4
#define ADC_MAX 4095


esp_now_peer_info_t slave;
uint8_t last_seq = 0;

// message structure
typedef struct __attribute__((packed)) { // pour éviter des trous dans la structure
                                        // mets les données les unes à côté de l'autre
    uint8_t magic_byte; // premier byte pour vérifier intégrité du message
    uint8_t seq;        // numéro du message
    uint8_t motor_speed;     // Vitesse du moteur (0 à 255) 

    uint8_t roll;
    uint8_t pitch;
    uint8_t yaw;

    bool arm;               // true = moteur autorisé, false = coupé
    unsigned long timestamp; // pour vérifier la vieillesse des messages
    uint8_t crc;   // checksum
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

uint8_t checkSum(uint8_t* data, size_t len) {

    uint8_t checksum = 0;
    for(size_t i = 0 ; i < len - 1 ; i++ ) {
        checksum += data[i];
    }
    return checksum;
}


void loop() {

  incomingMessage.magic_byte = 0xC7;
  incomingMessage.timestamp = millis();

  int adc_value = analogRead(THROTTLE_PIN);
  int yaw_value = analogRead(YAW_PIN);
  int roll_value = analogRead(ROLL_PIN);
  int pitch_value = analogRead(PITCH_PIN);

  incomingMessage.motor_speed = map(adc_value,0,ADC_MAX,0,255);

  incomingMessage.pitch =map(pitch_value,0,ADC_MAX,0,255);
  incomingMessage.roll =map(roll_value,0,ADC_MAX,0,255);
  incomingMessage.yaw =map(yaw_value,0,ADC_MAX,0,255);

  incomingMessage.arm = true;
  incomingMessage.seq = last_seq +1;

  incomingMessage.crc = checkSum((uint8_t*)&incomingMessage, sizeof(incomingMessage));


  esp_err_t result = esp_now_send(slave.peer_addr, (uint8_t*)&incomingMessage, sizeof(incomingMessage));

  if (result == ESP_OK) {

    Serial.printf("sent (seq=%d, gas=%d, roll=%d, pitch=%d, yaw=%d)\n",
                      incomingMessage.seq,
                      incomingMessage.motor_speed,
                      incomingMessage.roll,
                      incomingMessage.pitch,
                      incomingMessage.yaw);

    Serial.println("sent");
    last_seq = incomingMessage.seq;

  } else {
    Serial.println("error");
  }

  delay(20);


}
