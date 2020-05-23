#ifndef ROBOTS
#define ROBOTS

#include <ros/ros.h>
#include <iostream>

#include <aip_cf_commande_locale/RobotJoints.h>
#include <aip_cf_robots/MoveRobot.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Byte.h>
#include <vector>
#include "aip_cf_robots/FaireTacheMsg.h"
#include "aip_cf_robots/Msg_numrobot.h"
#include "aip_cf_commande_locale/SrvAddProduct.h"
#include "aip_cf_commande_locale/Msg_AddProduct.h"
#include "aip_cf_commande_locale/DeplacerPieceMsg.h"
#include "aip_cf_commande_locale/SrvAddProductPushBack.h"

// Etats robot
#define HAUT		1
#define BAS		-1
#define FERMEE		1
#define OUVERTE		-1
#define EN_POSITION	1

// Robots
#define ROBOT_1 1
#define ROBOT_2 2
#define ROBOT_3 3
#define ROBOT_4 4

// Postes
#define POSTE_1 1
#define POSTE_2 2
#define POSTE_3 3
#define POSTE_4 4
#define POSTE_5 5
#define POSTE_6 6
#define POSTE_7 7
#define POSTE_8 8


class RobotsInterface
{
 private:

  ros::Publisher pub_robot_position;
  ros::Publisher pub_robot_joints;
  ros::Publisher pub_pince_fermer;
  ros::Publisher pub_pince_ouvrir;
  ros::Publisher pub_pince_descendre;
  ros::Publisher pub_descendre;
  ros::Publisher pub_monter;
  ros::Publisher pub_controler_robot;
  ros::Publisher pub_faireTache;
  ros::Publisher pub_robot_transport1;
  ros::Publisher pub_robot_transport2;
  ros::Publisher pub_robot_transport3;
  ros::Publisher pub_robot_transport4;
  ros::Publisher pub_evacuer_piece;
  ros::Publisher pubProductAdd;
  ros::Publisher pub_deplacer_piece;

  ros::Subscriber sub_retourRobot;

  ros::ServiceClient client;
  ros::ServiceServer serverPushBack;
  aip_cf_commande_locale::SrvAddProduct srv;
  aip_cf_commande_locale::Msg_AddProduct msg0;
  aip_cf_commande_locale::DeplacerPieceMsg deplacer_msg;

  int nbRobot;
  int robotInit[4];
  int robotPosition[4];
  int robotBras[4];
  int bras[4];
  int robotPince[4];
  int pince[4];
  int robotTask[4][2];
  int robotMacroDeplacement[4];

  std::vector<int> produit_a_ajouter;

  aip_cf_robots::FaireTacheMsg tache_msg;

 public:
  RobotsInterface(ros::NodeHandle noeud, int nombre_robot);
  ~RobotsInterface();

  //Contrôles des robots
  void EnvoyerPosition(int numRobot, int numPosition);
  void EnvoyerAngles(int numRobot, int angle1, int angle2,
                     int angle3, int angle4, int angle5,
                     int angle6, int angle7);
  void ControlerRobot(int numRobot, int numPosition, int bras, int pince);
  void FermerPince(int numRobot);
  void OuvrirPince(int numRobot);
  void DescendreBras(int numRobot);
  void MonterBras(int numRobot);

  void DeplacerPiece(int num_robot, int positionA, int positionB);

  //Retour depuis les nodes des robots
  void RetourRobotCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg);
  
  //retourne 1 si le robot numRobot est initialisé
  int RobotInitialise(int numRobot);
  //retourne 1 si le robot numRobot est en position  
  int RobotEnPosition(int numRobot);
  //retourne 1 si le bras du robot numRobot est monté,
  // -1 si le bras du robot numRobot est descendu  
  int BrasEnPosition(int numRobot);
  //retourne 1 si la pince du robot numRobot est fermée,
  // -1 si elle est ouverte  
  int PinceEnPosition(int numRobot); 

  void RobotTransport(int num_robot, bool valeur);
  void FaireTache(int num_poste, int duree);
  int TacheFinie(int num_poste);
  void computeFromNumPoste(int num_poste,int tab[2]);
  int FinDeplacerPiece(int num_robot);
  void Evacuer();
  void AjouterProduit(int poste, int produit);
  int AjoutProduitEnAttente();

  bool ProductAddPushBack
  (aip_cf_commande_locale::SrvAddProductPushBack::Request& req,
   aip_cf_commande_locale::SrvAddProductPushBack::Response& rep);
};
#endif
