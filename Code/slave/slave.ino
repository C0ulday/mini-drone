#include <esp_now.h>
#include <WiFi.h>


#define MOTOR_PIN 3
#define PWM_FREQ 20000
#define PWM_RES  8          // 8 bits → 0..255

uint8_t last_seq;
unsigned long last_Rx = 0;


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




uint8_t checkSum(uint8_t* data, size_t len) {

    uint8_t checksum = 0;
    for(size_t i = 0 ; i < len - 1 ; i++ ) {
        checksum += data[i];
    }
    return checksum;
}

void launchMotor(){

    ledcWrite(MOTOR_PIN,incomingMessage.motorSpeed);
    Serial.printf("speed applied %d\n",incomingMessage.motorSpeed);

}

void stopMotor(){

    ledcWrite(MOTOR_PIN,0);
    Serial.printf("motor stopped\n");

}



void setup() {

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    Serial.println(WiFi.macAddress());
    
    if (esp_now_init() != ESP_OK) {

        Serial.println("Error initializing ESP-NOW\n");

        return;

    }

    ledcAttach(MOTOR_PIN, PWM_FREQ, PWM_RES);
    stopMotor();

    esp_now_register_recv_cb(OnDataRecv);

    Serial.println("ESP-NOW Receiver Ready\n");

}



void OnDataRecv(const esp_now_recv_info* mac, const uint8_t *incomingData, int len) {

    if (len != sizeof(Message)) {
        Serial.printf("Invalid size !\n");
        return;
    }

    Message temp;
    memcpy(&temp, incomingData, sizeof(temp));

    if(temp.magic_byte != 0xC7) {
        Serial.printf("Invalid magic byte !\n");
        return;
    }

    if(temp.crc != checkSum((uint8_t*)&temp, sizeof(temp))) {
        Serial.printf("Corrupted message, invalid checksum !\n");
        return;
    }

    if (last_seq !=0 && temp.seq != (last_seq+1)) {
        Serial.printf("Packets loss !\n");
    }
    // On peut désormais copier dans la structure finale
    memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
    last_Rx = millis();
    last_seq = incomingMessage.seq;

    if(incomingMessage.arm) {
        launchMotor();
    } else {
        stopMotor();
    } 


}



void loop() {
    
    if(last_Rx !=0 && millis() - last_Rx  > 300) {
        //Serial.println("connection lost...\n");
        stopMotor();
        last_Rx = 0;
        return;
    }

    delay(10);

}
