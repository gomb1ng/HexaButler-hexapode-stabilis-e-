#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

namespace Config {
    // Géométrie physique (mm)
    constexpr float BASE_RADIUS = 50.0f;     // 'd'
    constexpr float PLATFORM_RADIUS = 90.0f; // 'e'
    constexpr float SERVO_ARM = 50.0f;       // 'f'
    constexpr float TIE_ROD = 60.0f;         // 'g'
    constexpr float DEFAULT_HEIGHT = 70.0f;  // 'h'

    // Pré-calculs 
    constexpr float A_SQ = SERVO_ARM * SERVO_ARM;
    constexpr float B_SQ = TIE_ROD * TIE_ROD;
    constexpr float TWO_A = 2.0f * SERVO_ARM;
    constexpr float K_AL_KASHI = A_SQ - B_SQ;

    
    const int SERVO_PINS[3] = {9, 10, 11};
    const int SERVO_OFFSETS[3] = {0, -7, 7};
    constexpr uint32_t I2C_SPEED = 400000;          // 400kHz pour l'IMU
    constexpr uint32_t UPDATE_INTERVAL_US = 5000;   // 5ms (200Hz) pour la réactivité
};

#endif