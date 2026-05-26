#include <math.h>
#include <SoftwareSerial.h>
#include "constantes.h"
#include "Leg.h"
#include "Gates.h"


// --- CONFIGURATION BLUETOOTH ---
SoftwareSerial SSC32(3, 4);       // SSC32
SoftwareSerial BTSerial(10, 11);  // BLUETOOTH

// --- ETAT DU ROBOT ---
enum Mode {STOPPED, WALKING, ROTATING};
Mode currentMode = STOPPED;


// --- CONFIGURATION DES PATTES ---
Leg legs[6] = {
  Leg(0,   0.0,  8),   // Droite Milieu   4
  Leg(1,  45.0,  12),   // Droite Avant    8
  Leg(2, 135.0, 28),  // Gauche Avant   28
  Leg(3, 180.0, 24),  // Gauche Milieu  20
  Leg(4, 225.0, 16),  // Gauche Arrière 16
  Leg(5, 315.0,  0)    // Droite Arrière  0
};

Gates gates(0);


// --- PLANIFICATEUR D'IMAGES CLÉS (KEYFRAMES) ---
int currentStep = 0;                    // Index de l'image clé (0 à 7)
unsigned long lastFrameTime = 0;
unsigned int frameDuration = 1000; // Temps (ms) entre chaque image clé

float currentZrepos = Zrepos;
float currentPhi = 0.0;  // Angle de marche (0=Avant, 180=Arrière, 90=côté)
float currentRot = 1.0;  // 1 = Droite, -1 = Gauche
bool isActive = false;   // Bouton Power On/Off 

// --- GESTION DE L'INACTIVITÉ ET DU TRANSPORT ---
unsigned long lastStoppedTime = 0;  
bool alreadyAtRest = true;
const unsigned long INACTIVITY_TIMEOUT = 5000; 


void setup() {
  Serial.begin(115200);
  SSC32.begin(38400);
  BTSerial.begin(9600);
  SSC32.println("#4P1000#5P1000#6P1000T1000\r");
  resetPosition();// Position initiale
  delay(2000);
}


void loop() {
  // --- LECTURE BLUETOOTH EN CONTINU ---
  while (BTSerial.available()) {
    char c = BTSerial.read();
    Serial.println(c);
    handleAppChar(c);
  }
  
  // --- CADENCEMENT DES IMAGES CLÉS ---
  if (isActive && currentMode != STOPPED) {
    if (millis() - lastFrameTime >= frameDuration) {
      lastFrameTime = millis();
      executeKeyframe();
      currentStep++;
      if (currentStep >= 8) currentStep = 0;
    }
  }
  if (isActive && currentMode == STOPPED && !alreadyAtRest) {
    // Si le robot est allumé, à l'arrêt, pas encore au repos, et que 5 secondes se sont écoulées :
      if (millis() - lastStoppedTime >= INACTIVITY_TIMEOUT) {
        Serial.println("-> Inactivité détectée : Retour automatique à la position de repos");
        resetPosition();
        alreadyAtRest = true; // On verrouille pour pas resetPosition() en boucle
      }
    }
}


void executeKeyframe() {
  String commandGlobale = "";
  
  for (int i = 0; i < 6; i++) {
    // Alternance tripod : Groupe 1 (paires) et Groupe 2 (impaires déphasées de 4 pas)
    int localStep = (i % 2 == 0) ? currentStep : (currentStep + 4) % 8;
    Point3D coords;
    
    if (currentMode == WALKING) {
      coords = gates.GetKeyframePoint(localStep, currentPhi * PI / 180.0);
      Point3D coordsPatte = legs[i].transform(coords);
      Angles angles = legs[i].LegIK(coordsPatte);
      commandGlobale += legs[i].getCommand(angles);
    } 

    else if (currentMode == ROTATING) {
      coords = gates.GetKeyframePoint(localStep, currentPhi * PI / 180.0);
      Point3D coordsPatte = legs[0].transform(coords); 
      Angles angles = legs[i].LegIK(coordsPatte);
      commandGlobale += legs[i].getCommand(angles);
    }

    /*if ((i == 0 || i == 3) && currentMode == WALKING && (currentPhi == 90.0 || currentPhi == -90.0)) {
      coordsPatte.y = 0.0;
      if (coordsPatte.x < 70.0)  coordsPatte.x = 70.0;  
      if (coordsPatte.x > 140.0) coordsPatte.x = 140.0;
    }
    */
  }
  commandGlobale += "T" + String(frameDuration) + "\r";
  SSC32.print(commandGlobale);
}



void resetPosition() {
  //SSC32.println("#0P1500#1P1500#2P1500#8P1500#9P1500#10P1500#8P1500#9P1500#10P1500#16P1500#17P1500#18P1500#24P1500#25P1500#26P1500#28P1500#29P1500#30P1500T1000\r"); 
  String commandGlobale = "";
  
  // Coordonnées au repos
  Point3D coordsRepos;
  coordsRepos.x = Xrepos;
  coordsRepos.y = Yrepos;
  coordsRepos.z = currentZrepos;

  for (int i = 0; i < 6; i++) {
    Point3D coordsPatte = legs[i].transform(coordsRepos);
    Angles angles = legs[i].LegIK(coordsPatte);
    commandGlobale += legs[i].getCommand(angles);
  }
  commandGlobale += "T1000\r";
  
  SSC32.print(commandGlobale);
  Serial.println("Le robot est en position de repos dynamique.");
}

void compactPosition(){
  SSC32.print("#0P2200#1P2200#2P1100 #8P2200#9P2200#10P1100 #12P2200#13P2200#14P1000T1100\r");
  SSC32.println("#16P2200#17P700#18P2000 #24P2200#25P700#26P2000 #28P2200#29P700#30P2000T1000\r");
}



// --- LOGIQUE DES COMMANDES ---
void handleAppChar(char c) {
  static char actionEnCours = '\0'; 
  if (c == '\n' || c == '\r' || c == ' ') return;

  if (actionEnCours == 'V' || actionEnCours == 'H') {
    int pourcentage = (unsigned char)c; // On récupère la valeur (0-255)
    if (c >= '0' && c <= '9' && pourcentage < 10) {
    }
    pourcentage = constrain(pourcentage, 10, 100);

    if (actionEnCours == 'V') {
      frameDuration = map(pourcentage, 10, 100, 1500, 200); 
      Serial.print("-> VITESSE : ");
      Serial.print(pourcentage);
      Serial.print("% ("); Serial.print(frameDuration); Serial.println(" ms)");
    } 
    else if (actionEnCours == 'H') {
      currentZrepos = map(pourcentage, 10, 100, -40, -85); 
      Serial.print("-> HAUTEUR : ");
      Serial.print(pourcentage);
      Serial.print("% ("); Serial.print(currentZrepos); Serial.println(" mm)");
      
      if (currentMode == STOPPED && isActive) {
        resetPosition();
      }
    }

    actionEnCours = '\0'; 
    return;
  }


  if (c == 'V' || c == 'H') {
    actionEnCours = c; 
    return;
  }

  // 4. Gestion du bouton Power ON/OFF
  if (c == 'O') { 
    isActive = !isActive;
    if (isActive) {
      Serial.println("-> ROBOT ACTIF");
      currentMode = STOPPED;  
      alreadyAtRest = true; // Déjà au repos
    } else {
      Serial.println("-> ROBOT ETEINT");
      compactPosition(); 
      currentMode = STOPPED;  
    }
    return; 
  }

  // 5. Gestion des boutons de direction
  if (isActive) {
    switch (c) {
      case 'S': currentMode = STOPPED;   Serial.println("-> Mode: STOP");
        lastStoppedTime = millis(); // ON LANCE LE CHRONO ICI
        alreadyAtRest = false;
      break;
      case 'F': currentMode = WALKING;   currentPhi = 0.0;    Serial.println("-> Marche Avant"); break;
      case 'B': currentMode = WALKING;   currentPhi = 180.0;  Serial.println("-> Marche Arriere"); break;
      case 'L': currentMode = WALKING;   currentPhi = -90.0;  Serial.println("-> Pas Gauche"); break;
      case 'R': currentMode = WALKING;   currentPhi = 90.0;   Serial.println("-> Pas Droite"); break;
      case 'G': currentMode = ROTATING;  currentPhi = 0.0;    currentRot = -1.0;  Serial.println("-> Rotation Gauche"); break;
      case 'D': currentMode = ROTATING;  currentPhi = 180.0;  currentRot = 1.0;   Serial.println("-> Rotation Droite"); break;
    }
  }
}