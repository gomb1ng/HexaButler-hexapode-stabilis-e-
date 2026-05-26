#include <Wire.h>
#include <MPU6050_light.h>
#include "StewartPlatform.h"

MPU6050 mpu(Wire);
StewartPlatform platform;
uint32_t lastMicros = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(Config::I2C_SPEED); // Boost I2C pour la réactivité

    if(mpu.begin() != 0) {
        Serial.println(F("Erreur MPU6050"));
        while(1);
    }

    Serial.println(F("Calibration IMU..."));
    delay(1000);
    mpu.calcOffsets(); 
    
    platform.begin();
    lastMicros = micros();
}

void loop() {
    mpu.update();

    uint32_t currentMicros = micros();
    // Boucle à haute fréquence constante
    if (currentMicros - lastMicros >= Config::UPDATE_INTERVAL_US) {
        lastMicros = currentMicros;

        // Stabilisation active : compensation des angles mesurés
        // On multiplie par un gain (ici 1.0) pour ajuster la nervosité
        float targetPitch = -mpu.getAngleY(); 
        float targetRoll = -mpu.getAngleX();

        Serial.print("target pitch : ");
        Serial.println (targetPitch);
        Serial.print("target roll : ");
        Serial.println(targetRoll);

        // Mise à jour instantanée des moteurs
        platform.update(targetPitch, targetRoll, Config::DEFAULT_HEIGHT);
    }
}