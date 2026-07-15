#include <esp_now.h>
#include <WiiFi.h>

typedef struct message {
    char deviceName[32];    // Nom de l'émetteur
    uint8_t motorSpeed;     // Vitesse du moteur (0 à 255) 
    bool arm;               // true = moteur autorisé, false = coupé
    unsigned long timestamp;
} struct_message;

message incomingMessage;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

    memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

    

    Serial.println("=== Message Received ===");

    Serial.printf("Device: %s\n", incomingMessage.deviceName);

    Serial.printf("motorSpeed: %d\n", incomingMessage.motorSpeed);

    Serial.printf("arm: %.2f°C\n", incomingMessage.arm);

    Serial.printf("Timestamp: %lu\n", incomingMessage.timestamp);

    Serial.println("========================");

}

void setup() {

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    Serial.println(WiFi.macAddress());

    

    if (esp_now_init() != ESP_OK) {

        Serial.println("Error initializing ESP-NOW");

        return;

    }

    esp_now_register_recv_cb(OnDataRecv);

    Serial.println("ESP-NOW Receiver Ready");

}


void loop() {

  delay(1000);

}
