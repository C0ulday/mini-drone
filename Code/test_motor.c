/*
 * Test canal moteur — ESP32-C3 Super Mini
 * Moteur M1 sur GPIO3 (gate AO3400 via 100R, pulldown 10k)
 * SS14 flyback aux bornes du moteur, cathode (barre) vers VBAT
 *
 * MODE 1 : ON/OFF lent       -> verification des tensions au multimetre
 * MODE 2 : rampe PWM 20 kHz  -> validation de la commande proportionnelle
 */

#define MOTOR_PIN 3
#define TEST_MODE 1      

#define PWM_FREQ 20000
#define PWM_RES  8         // 8 bits -> duty 0..255

void setup() {
    Serial.begin(115200);
    delay(500);

    #if TEST_MODE == 1
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, LOW);
    Serial.println("MODE 1 : ON/OFF : mesure les tensions");
    #else
    ledcAttach(MOTOR_PIN, PWM_FREQ, PWM_RES);   // core ESP32 3.x
    ledcWrite(MOTOR_PIN, 0);
    Serial.println("MODE 2 : rampe PWM 20 kHz");
    #endif
    }

    void loop() {
    #if TEST_MODE == 1
    digitalWrite(MOTOR_PIN, HIGH);
    Serial.println("GATE HIGH (ON)  -> drain ~0V, moteur tourne");
    delay(3000);

    digitalWrite(MOTOR_PIN, LOW);
    Serial.println("GATE LOW  (OFF) -> drain ~VBAT, moteur stop");
    delay(3000);

    #else
    for (int d = 0; d <= 255; d += 5) {        // montee
        ledcWrite(MOTOR_PIN, d);
        Serial.printf("duty = %3d / 255\n", d);
        delay(100);
    }
    for (int d = 255; d >= 0; d -= 5) {        // descente
        ledcWrite(MOTOR_PIN, d);
        Serial.printf("duty = %3d / 255\n", d);
        delay(100);
    }
    delay(500);
    #endif
}