#include <Wire.h>
#include <MPU6050_light.h>
#include <SoftwareSerial.h>
#include "StewartPlatform.h"

MPU6050 mpu(Wire);
StewartPlatform platform;
SoftwareSerial SSC32(3, 4);
uint32_t lastMicros = 0;

void setup() {
    Serial.begin(115200);
    SSC32.begin(38400);
    SSC32.println("#0P1500#1P1500#2P1500#8P1500#9P1500#10P1500#12P1500#13P1500#14P1500#16P1500#17P1500#18P1500#24P1500#25P1500#26P1500#28P1500#29P1500#30P1500T1000\r"); 
  
    //Serial.print("#5P1500T1000\r");
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
    if (currentMicros - lastMicros >= Config::UPDATE_INTERVAL_US) {
        lastMicros = currentMicros;

        // Stabilisation active : compensation des angles mesurés
        float targetPitch = -mpu.getAngleY(); 
        float targetRoll = -mpu.getAngleX();

        Serial.print("target pitch : ");
        Serial.println (targetPitch);
        Serial.print("target roll : ");
        Serial.println(targetRoll);

        // Mise à jour instantanée des moteurs
        platform.update(targetPitch, targetRoll, Config::DEFAULT_HEIGHT, SSC32);
    }
}