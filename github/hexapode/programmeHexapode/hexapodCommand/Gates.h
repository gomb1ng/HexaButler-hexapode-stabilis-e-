#ifndef GATES_H
#define GATES_H

#include <Arduino.h>
#include "constantes.h"

class Gates{

  private:
    int _id;

  public:
    Gates(int id);
    Point3D Walk(double phase, float phirad);
    Point3D GetKeyframePoint(int step, float phirad);
};

#endif