#include "Leg.h"

Leg :: Leg(int id, float beta, int pin):
  _id(id), _beta(beta), _pin(pin) {}



Point3D Leg::transform(Point3D pBody) {
  Point3D pLeg;
  float rad = -_beta * PI / 180.0; 

  // On calcule uniquement le déplacement par rapport au repos
  float deltaX = pBody.x - Xrepos;
  float deltaY = pBody.y - Yrepos;

  // On fait pivoter ce déplacement (le vecteur du pas) --> Application de la matrice de rotation
  float rotatedDeltaX = deltaX * cos(rad) - deltaY * sin(rad);
  float rotatedDeltaY = deltaX * sin(rad) + deltaY * cos(rad);

  // On rajoute ce déplacement pivoté à notre point de repos LOCAL
  pLeg.x = Xrepos + rotatedDeltaX;
  pLeg.y = Yrepos + rotatedDeltaY;
  pLeg.z = pBody.z;

  return pLeg;
}


Angles Leg::LegIK(Point3D position){
  Angles a; // On crée une structure locale pour les angles

  bool isLeft = (_beta > 90 && _beta < 270); //on regarde si la patte est à droite ou à gauche

float r = sqrt(position.x * position.x + position.y * position.y);
  
  float rMin = 70.0;  // Empêche la patte de trop rentrer dans le corps 
  float rMax = 140.0; // Empêche la patte de trop s'étirer au sol

  if (r < 0.001) { // Évite une division par zéro
    position.x = rMin;
    position.y = 0.0;
  } else if (r < rMin) {
    position.x = (position.x / r) * rMin;
    position.y = (position.y / r) * rMin;
  } else if (r > rMax) {
    position.x = (position.x / r) * rMax;
    position.y = (position.y / r) * rMax;
  }



  float theta1 = atan2(position.y, position.x); // angle moteur 1 (plan XY)

  float r2 = position.x / cos(theta1) - L1;
  float r1 = sqrt(position.z*position.z + r2*r2);

  float ratio1 = (r1*r1 + L2*L2 - L3*L3) / (2 * L2 * r1);
  float ratio2 = (L3*L3 + L2*L2 - r1*r1) / (2 * L2 * L3);
  ratio1 = constrain(ratio1, -1.0, 1.0);
  ratio2 = constrain(ratio2, -1.0, 1.0);

  float phi1 = acos(ratio1);
  float phi2 = atan2(position.z, r2);
  float phi3 = acos(ratio2);

  float theta2 = phi1 + phi2; // moteur 2 (plan XZ)
  float theta3 = phi3;        // moteur 3 (plan XZ) 

  a.theta1 = theta1 * 180/PI + 90;   
  a.theta2 = theta2 * 180/PI + 90;
  a.theta3 = theta3 * 180/PI + 47; //on ajoute l'offset dû à la forme de la patte

  if (isLeft == true){
    //a.theta1 = 180 - a.theta1;
    a.theta2 = 180 - a.theta2;
    a.theta3 = 180 - a.theta3;
  }

  a.theta1 = constrain(a.theta1, 0, 180);
  a.theta2 = constrain(a.theta2, 0, 180);
  a.theta3 = constrain(a.theta3, 0, 180);

  return a;
}

String Leg::getCommand(Angles angles){
    //----- commande pour SSC32 -----
  int pulse1 = map(angles.theta1, 0, 180, 800, 2200);
  int pulse2 = map(angles.theta2, 0, 180, 800, 2200);
  int pulse3 = map(angles.theta3, 0, 180, 800, 2200);
  String command = ""; // On prépare une chaîne de caractères vide
  command += "#" + String(_pin) + " P" + String(pulse1)  + " " + "#" + String(_pin + 1) + " P" + String(pulse2)  + " " + "#" + String(_pin + 2) + " P" + String(pulse3);
  command += " "; 
  return command;
}

float Leg::getBeta(){
  return _beta;
}