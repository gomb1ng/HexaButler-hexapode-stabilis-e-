#include "StewartPlatform.h"
#include <math.h>

StewartPlatform::StewartPlatform() {
    for(int i = 0; i < 3; i++) {
        float angle = radians(i * 120.0f + 90.0f);
        M[i] = {cosf(angle) * Config::BASE_RADIUS, sinf(angle) * Config::BASE_RADIUS, 0.0f};
        U[i] = {cosf(angle) * Config::PLATFORM_RADIUS, sinf(angle) * Config::PLATFORM_RADIUS, 0.0f};
    }
}

void StewartPlatform::begin() {
    for(int i = 0; i < 3; i++) servos[i].attach(Config::SERVO_PINS[i]);
}

void StewartPlatform::update(float pitchDeg, float rollDeg, float heave) {
    float p = radians(pitchDeg);
    float r = radians(rollDeg);
    
    // Calcul unique des sinus/cosinus pour tout le cycle
    float cp = cosf(p); float sp = sinf(p);
    float cr = cosf(r); float sr = sinf(r);

    // Coefficients de la matrice R 
    // R[0][0]=cp, R[0][1]=sp*sr, R[1][1]=cr, R[1][2]=-sr, etc. 
    
    for(int i = 0; i < 3; i++) {
        // Uz est nul, on ignore R_i2 * 0
        float px = cp * U[i].x + (sp * sr) * U[i].y;
        float py = cr * U[i].y; 
        float pz = -sp * U[i].x + (cp * sr) * U[i].y + heave;

        // Vecteur bras Li 
        float lx = px - M[i].x;
        float ly = py - M[i].y;
        float lz = pz - M[i].z;
        
        float Li_sq = lx*lx + ly*ly + lz*lz;
        float Li = sqrtf(Li_sq);

        // cos(alpha) = (Li² + a² - b²) / (2 * a * Li)
        float cosAlpha = (Li_sq + Config::K_AL_KASHI) / (Config::TWO_A * Li);
        
        // Clamp pour la stabilité numérique
        if (cosAlpha > 1.0f) cosAlpha = 1.0f;
        else if (cosAlpha < -1.0f) cosAlpha = -1.0f;
        
        // Conversion en angle servo et écriture directe
        servos[i].write(degrees(acosf(cosAlpha)) + Config::SERVO_OFFSETS[i]);
    }
}