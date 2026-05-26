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
}

void StewartPlatform::update(float pitchDeg, float rollDeg, float heave, Stream &SSC32) {
    float p = radians(pitchDeg);
    float r = radians(rollDeg);
    
    // Calcul unique des sinus/cosinus pour tout le cycle
    float cp = cosf(p); float sp = sinf(p);
    float cr = cosf(r); float sr = sinf(r);

    // Coefficients de la matrice R 
    // R[0][0]=cp, R[0][1]=sp*sr, R[1][1]=cr, R[1][2]=-sr, etc. 
    int targetPW[3];
    
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

        int angleDeg = (int)degrees(acosf(cosAlpha))+Config::SERVO_OFFSETS[i];
        int pw = map(angleDeg, 0, 180, 1000, 2000);
        pw = constrain(pw, Config::PW_MIN, Config::PW_MAX);
        targetPW[i] = pw;
    }

        // --- ENVOI DE LA COMMANDE SYNCHRONE A LA SSC-32 ---
    SSC32.print("#");  SSC32.print(Config::SSC_CHANNELS[0]); SSC32.print(" P"); SSC32.print(targetPW[0]);
    SSC32.print(" #"); SSC32.print(Config::SSC_CHANNELS[1]); SSC32.print(" P"); SSC32.print(targetPW[1]);
    SSC32.print(" #"); SSC32.print(Config::SSC_CHANNELS[2]); SSC32.print(" P"); SSC32.print(targetPW[2]);
    SSC32.print("\r"); // Déclenchement simultané
    /*Serial.print("#");  Serial.print(Config::SSC_CHANNELS[0]); Serial.print(" P"); Serial.print(targetPW[0]);
    Serial.print(" #"); Serial.print(Config::SSC_CHANNELS[1]); Serial.print(" P"); Serial.print(targetPW[1]);
    Serial.print(" #"); Serial.print(Config::SSC_CHANNELS[2]); Serial.print(" P"); Serial.print(targetPW[2]);
    Serial.print("\r");*/
}
