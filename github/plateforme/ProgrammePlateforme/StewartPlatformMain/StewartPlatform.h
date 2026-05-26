#ifndef STEWART_PLATFORM_H
#define STEWART_PLATFORM_H

#include <Arduino.h>
#include <Servo.h>
#include "Constants.h"

struct Point3D { float x, y, z; };

class StewartPlatform {
private:
    Servo servos[3];
    Point3D M[3]; // Base
    Point3D U[3]; // Plateau (coordonnées locales)
    
public:
    StewartPlatform();
    void begin();
    // Méthode optimisée pour le calcul matriciel
    void update(float pitchDeg, float rollDeg, float heave);
};

#endif