#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <Arduino.h>
#include <math.h>


//nom des pattes
const int DROITE_MILIEU = 0;
const int DROITE_AVANT = 1;
const int GAUCHE_AVANT = 2;
const int GAUCHE_MILIEU = 3;
const int GAUCHE_ARRIERE = 4;
const int DROITE_ARRIERE = 5;// leg[5]

//dimension des pattes
const int L1 = 48;
const int L2 = 70;
const int L3 = 110;

//dimension gate
const int A = 50;
const int T = 100;

//position de repos d'une patte
const int Xrepos = 120;
const int Yrepos = 0;
const int Zrepos = -70;


const int R = T/2;



struct Point3D {
  float x, y, z;
};
struct Angles {
  float theta1, theta2, theta3;
};



#endif