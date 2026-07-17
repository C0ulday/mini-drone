#include <esp_now.h>
#include <WiFi.h>


#define MOTOR_PIN 3
#define PWM_FREQ 20000
#define PWM_RES  8          // 8 bits → 0..255

typedef struct {
    char deviceName[32];    // Nom de l'émetteur
    uint8_t motorSpeed;     // Vitesse du moteur (0 à 255) 
    bool arm;               // true = moteur autorisé, false = coupé
    unsigned long timestamp;
} Message;

Message incomingMessage;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

    memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

    if(millis() - incomingMessage.timestamp > 300) {
        Serial.println("connection lost...\n");
        stopMotor();
        return;
    }

    if(!incomingMessage.arm){
        Serial.println("motor isn't armed...\n");
        stopMotor();
        return;
    }
    
    // if there is still a link with the remote

    launchMotor();

    Serial.println("=== Message Received ===");

    Serial.printf("Device: %s\n", incomingMessage.deviceName);

    Serial.printf("arm: %s\n", incomingMessage.arm ? "ARMED" : "DISARMED");

    Serial.printf("Timestamp: %lu\n", incomingMessage.timestamp);

    Serial.println("========================");

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


void loop() {

    delay(1000);

}
