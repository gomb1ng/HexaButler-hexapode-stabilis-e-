#ifndef LEG_H
#define LEG_H

#include <Arduino.h>
#include "constantes.h"
#include "Gates.h"

class Leg{
  private:
    int _id;
    float _beta; //angle de la patte par rapport au corps
    //float _Xoffset; //distance centre robot / épaule en X
    //float _Yoffset;
    int _pin; //premier Pin sur le ssc32

  public:
    Leg(int id, float beta, int _pin);
    Point3D transform(Point3D pBody);
    Angles LegIK(Point3D position);
    String getCommand(Angles angles);
    float getBeta();
};

#endif