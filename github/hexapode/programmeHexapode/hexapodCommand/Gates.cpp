#include "Gates.h"

Gates::Gates(int id) : _id(id) {}

Point3D Gates::GetKeyframePoint(int step, float phirad) {
  Point3D position;

  // 1. Définition des coefficients de mouvement pour les 8 étapes (0 à 7)
  // kX gère l'axe horizontal (de -1.0 à 1.0)
  // kZ gère la hauteur de la patte (0.0 = au sol, 1.0 = levée au maximum)
  
  // Étapes 0 à 3 : La patte est en l'air et avance (Swing)
  // Étapes 4 à 7 : La patte est au sol et pousse le robot (Stance)
  const float kX[8] = {-1.0, -0.33,  0.33,  1.0,  0.5,  0.0, -0.5, -1.0};
  const float kZ[8] = { 1.0,  1.0,   1.0,   0.0,  0.0,  0.0,  0.0,  0.0};

  // 2. Calcul de la direction du pas
  double Xcible = R * sin(phirad);
  double Ycible = R * cos(phirad);

  // 3. Application de l'image clé courante
  position.x = Xrepos + kX[step] * Xcible;
  position.y = Yrepos + kX[step] * Ycible;
  position.z = Zrepos + kZ[step] * A;

  return position;
}