#include "RobotsInterface.h"

#include <ros/ros.h>
#include <iostream>
#include <unistd.h>

using namespace std;

//Messages
#include <std_msgs/Int32.h>

using namespace std;

#define RESET   "\033[0m"
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */

RobotsInterface::RobotsInterface(ros::NodeHandle noeud, int nombre_robot)
{
  nbRobot=nombre_robot;

  //Mise à 0 des retours des robots et des traitements
  for(int i=0;i<4;i++)
  {
    robotInit[i]=0;
    robotBras[i]=0;
    robotPosition[i]=0;
    robotPince[i]=0;

    robotTask[i][0]=0;
    robotTask[i][1]=0;
    robotMacroDeplacement[i]=1;

    bras[i]=-10;
    pince[i]=-10;
  }


  pub_robot_position=noeud.advertise<aip_cf_robots::Msg_numrobot>
                     ("/commande/Simulation/SendPositionRobot",10);
  pub_robot_joints=noeud.advertise<aip_cf_commande_locale::RobotJoints>
                   ("/commande/Simulation/SendJointsRobot",10);
  pub_pince_fermer=noeud.advertise<aip_cf_robots::Msg_numrobot>
                   ("/commande/Simulation/FermerPinceRobot",10);
  pub_pince_ouvrir=noeud.advertise<aip_cf_robots::Msg_numrobot>
                   ("/commande/Simulation/OuvrirPinceRobot",10);
  pub_descendre=noeud.advertise<aip_cf_robots::Msg_numrobot>
                ("/commande/Simulation/DescendreBras",10);
  pub_monter=noeud.advertise<aip_cf_robots::Msg_numrobot>
             ("/commande/Simulation/MonterBras",10);
  pub_controler_robot=noeud.advertise<aip_cf_robots::MoveRobot>
                      ("/commande/Simulation/ControlerBras",10);
  pub_faireTache=noeud.advertise<aip_cf_robots::FaireTacheMsg>
                 ("/commande/Simulation/faireTache", 10);
  pub_evacuer_piece=noeud.advertise<std_msgs::Byte>
                    ("/commande/Simulation/Evacuer",10);
  pubProductAdd= noeud.advertise<aip_cf_commande_locale::Msg_AddProduct>
                 ("/commande_locale/AddProduct",10);
  pub_deplacer_piece= noeud.advertise<aip_cf_commande_locale::DeplacerPieceMsg>
                      ("/commande/Simulation/DeplacerPiece",10);



  //Retour des robots vers la commande
  sub_retourRobot =
      noeud.subscribe("/commande/Simulation/retourCommande",
                      100, &RobotsInterface::RetourRobotCallback,this);
  
  client =
    noeud.serviceClient<aip_cf_commande_locale::SrvAddProduct>
      ("srv_add_product");
  serverPushBack =
      noeud.advertiseService
      ("srv_add_product_push_back",
       &RobotsInterface::ProductAddPushBack,this);

  ros::Duration(1).sleep();
}

RobotsInterface::~RobotsInterface()
{
}

/*** Envoyer les robots automatiquement ***/
//Fonction pour envoyer un robot choisi dans une position prédéfinie
void RobotsInterface::EnvoyerPosition(int numRobot, int numPosition)
{
  if(numRobot>=1 && numRobot<=nbRobot)
  {
    aip_cf_robots::Msg_numrobot msg;
    //Numéro de la position souhaitée
    if(numPosition<1 || numPosition>4)
      cout <<  BOLDMAGENTA
           << "Le numero de la position souhaitee doit etre"
           << " compris entre 1 et 4."
           << RESET << endl;
    else
      msg.data = numPosition;
    msg.num_robot=numRobot;
    robotPosition[numRobot-1]=0;
    pub_robot_position.publish(msg);
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}

/*** Envoyer les robots manuellement ***/
//Fonction pour envoyer un robot choisi dans une position définie par l'utilisateur
void RobotsInterface::
EnvoyerAngles(int numRobot, int angle1, int angle2,
              int angle3, int angle4, int angle5, int angle6, int angle7)
{
  // Publication du message vers le node robot
  // en fonction du numéro de robot à commander
  if(numRobot>=1 && numRobot<=nbRobot)
  {
    aip_cf_commande_locale::RobotJoints msg;

    //Angles (en degrés) choisis par l'utilisateur
    msg.joint1 = angle1;
    msg.joint2 = angle2;
    msg.joint3 = angle3;
    msg.joint4 = angle4;
    msg.joint5 = angle5;
    msg.joint6 = angle6;
    msg.joint7 = angle7;
    msg.num_robot=numRobot;
    robotPosition[numRobot-1]=0;
    pub_robot_joints.publish(msg);
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}

/*** Contrôler tous les mouvements des robots ***/
//Fonction permettant d'envoyer une position prédéfinie,
// l'état souhaité du bras et l'état souhaité de la pince d'un robot choisi
void RobotsInterface::
ControlerRobot(int numRobot, int numPosition, int bras, int pince)
{
  aip_cf_robots::MoveRobot controle;

  controle.position = numPosition;
  controle.bras = bras;
  controle.pince = pince;
  controle.num_robot=numRobot;

  if(numPosition<1 || numPosition>4)
  {
    cout <<  BOLDMAGENTA
         << "Le numero de la position doit etre compris entre 1 et 4."
         << RESET << endl;
    // on a que 4 position possibles, ces position sont 1 2 3 et 4
  }

  else if(bras != -1 && bras != 1)
  {
    cout <<  BOLDMAGENTA
         << "Cet etat du bras est inaccessible." << RESET << endl;
    // on a que 2 états du bras possibles, ces états sont -1 et +1
  }

  else if(pince != -1 && pince != 1)
  {
    cout <<  BOLDMAGENTA
         << "Cet etat de la pince est inaccessible." << RESET << endl;
    // on a que 2 états de pince possibles, ces états sont -1 et +1
  }

  else if(numRobot<1 || numRobot>nbRobot)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
    // on a que nbRobot robots
  }
  else
  {
    robotPosition[numRobot-1]=0;
    pub_controler_robot.publish(controle);
  }
}

/*** Fermer les pinces ***/
//Fonction pour fermer la pince d'un robot choisi
void RobotsInterface::FermerPince(int numRobot)
{
  if(numRobot>=1 && numRobot<=nbRobot)
  {
    robotPince[numRobot-1]=0;
    aip_cf_robots::Msg_numrobot msg;
    msg.data=1;
    msg.num_robot=numRobot;
    pub_pince_fermer.publish(msg);
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}

/*** Ouvrir les pinces ***/
//Fonction pour ouvrir la pince d'un robot choisi
void RobotsInterface::OuvrirPince(int numRobot)
{
  if(numRobot>=1 && numRobot<=nbRobot)
  {
    robotPince[numRobot-1]=0;
    aip_cf_robots::Msg_numrobot msg;
    msg.data=0;
    msg.num_robot=numRobot;
    pub_pince_ouvrir.publish(msg);
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}

/*** Descendre les bras ***/
//Fonction pour descendre le bras d'un robot choisi
void RobotsInterface::DescendreBras(int numRobot)
{
  if(numRobot>=1 && numRobot<=nbRobot)
  {
    robotBras[numRobot-1]=0;
    aip_cf_robots::Msg_numrobot msg;
    msg.data=1;
    msg.num_robot=numRobot;
    pub_descendre.publish(msg);
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}

/*** Monter les bras ***/
//Fonction pour monter le bras d'un robot choisi
void RobotsInterface::MonterBras(int numRobot)
{
  if(numRobot>=1 && numRobot<=nbRobot)
  {
    robotBras[numRobot-1]=0;
    aip_cf_robots::Msg_numrobot msg;
    msg.data=0;
    msg.num_robot=numRobot;
    pub_monter.publish(msg);
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}


/*** Retour des robots ***/
//Callback pour le retour des informations relatives au robot 1
void RobotsInterface::RetourRobotCallback
(const aip_cf_robots::Msg_numrobot::ConstPtr& msg)
{
  switch(msg->data)
  {
    case 0:
      cout <<  BOLDCYAN << "Robot "
           << msg->num_robot << " : Initialise"<< RESET << endl;
      robotInit[msg->num_robot-1]=1;
      break;

    case 1:
      cout <<  BOLDCYAN
           << "Robot " << msg->num_robot << " : Mouvement non effectue"
           << RESET << endl;
      robotPosition[msg->num_robot-1]=0;
      break;

    case 2:
      cout <<  BOLDCYAN
           << "Robot " << msg->num_robot << " : en position" << RESET << endl;
      robotPosition[msg->num_robot-1]=EN_POSITION;
      break;

    case 3:
      cout <<  BOLDCYAN
           << "Robot " << msg->num_robot << " : Bras bloque" << RESET << endl;
      robotBras[msg->num_robot-1] = 0;
      break;

    case 4:
      cout << BOLDCYAN
           << "Robot " << msg->num_robot << " : Bras descendu" << RESET << endl;
      robotBras[msg->num_robot-1] = BAS;
      break;

    case 5:
      cout << BOLDCYAN
           << "Robot " << msg->num_robot << " : Bras monte" << RESET << endl;
      robotBras[msg->num_robot-1] = HAUT;
      break;

    case 6:
      cout << BOLDCYAN
           << "Robot " << msg->num_robot << " : Pince fermee" << RESET << endl;
      robotPince[msg->num_robot-1] = FERMEE;
      break;

    case 7:
      cout << BOLDCYAN
           << "Robot " << msg->num_robot << " : Pince ouverte" << RESET << endl;
      robotPince[msg->num_robot-1] = OUVERTE;
      break;

    case 8:
      cout << BOLDCYAN
           << "Robot "
           << msg->num_robot
           << " : Tache du poste en position 1 terminee" << RESET << endl;
      robotTask[msg->num_robot-1][0]=1;
      break;

    case 9:
      cout << BOLDCYAN
           << "Robot "
           << msg->num_robot
           << " : Tache du poste en position 4 terminee" << RESET << endl;
      robotTask[msg->num_robot-1][1]=1;
      break;

    case 10:
      cout << BOLDCYAN
           << "Robot "
           << msg->num_robot
           << " : Deplacer piece terminee" << RESET << endl;
      robotMacroDeplacement[msg->num_robot-1]=1;
      break;
  }
}

//Fonction permettant de savoir si le robot choisi est initalisé
int RobotsInterface::RobotInitialise(int numRobot)
{
  int Etat;
  if(numRobot<1 || numRobot>nbRobot)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
    return 1;
  }
  else
    Etat=robotInit[numRobot-1];
  return Etat;
}

//Fonction permettant de savoir si le robot choisi est en position
int RobotsInterface::RobotEnPosition(int numRobot)
{
  int Etat;
  if(numRobot<1 || numRobot>nbRobot)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
    return 1;
  }
  else
    Etat=robotPosition[numRobot-1];
  return Etat;
}

//Fonction permettant de savoir si le bras du robot choisi est en position
int RobotsInterface::BrasEnPosition(int numRobot)
{
  int Etat;
  if(numRobot<1 || numRobot>nbRobot)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
    return 1;
  }
  else
    Etat=robotBras[numRobot-1];

  return Etat;
}

//Fonction permettant de savoir si la pince du robot choisi est en position
int RobotsInterface::PinceEnPosition(int numRobot)
{
  int Etat;
  if(numRobot<1 || numRobot>nbRobot)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
    return 1;
  }
  else
    Etat=robotPince[numRobot-1];

  return Etat;
}

int RobotsInterface::TacheFinie(int num_poste)
{
  int Etat;
  if(num_poste<1 || num_poste>nbRobot*2)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du poste doit etre compris entre 1 et "
         << nbRobot*2 << "." << RESET << endl;
    return 1;
  }
  else
  {
    int tab[2];
    computeFromNumPoste(num_poste,tab);
    int num_robot=tab[0];
    int position=tab[1];

    if(position==1)
      Etat=robotTask[num_robot-1][0];
    if(position==4)
      Etat=robotTask[num_robot-1][1];
  }

  return Etat;
}

int RobotsInterface::FinDeplacerPiece(int numRobot)
{
  int Etat;
  if(numRobot<1 || numRobot>nbRobot)
  {
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
    return 1;
  }
  else
    Etat=robotMacroDeplacement[numRobot-1];

  return Etat;
}


//Macro-fonction. Utilise les blocs élémentaires définis plus haut
void RobotsInterface::DeplacerPiece(int num_robot, int positionA, int positionB)
{
  if(num_robot>=1 && num_robot<=nbRobot)
  {
    if ((positionA<5 && positionA>0)&&(positionB<5 && positionB>0))
    {
      robotMacroDeplacement[num_robot-1]=0;
      deplacer_msg.num_robot = num_robot;
      deplacer_msg.positionA = positionA;
      deplacer_msg.positionB = positionB;
      pub_deplacer_piece.publish(deplacer_msg);
    }
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du robot doit etre compris entre 1 et "
         << nbRobot << "." << RESET << endl;
}

void RobotsInterface::computeFromNumPoste(int num_poste, int tab[2])
{
  // tab[0]=num_robot  tab[1]=position
  switch(num_poste)
  {
    case 1:
      tab[0]=1;
      tab[1]=4;
      break;
    case 2:
      tab[0]=1;
      tab[1]=1;
      break;
    case 3:
      tab[0]=2;
      tab[1]=1;
      break;
    case 4:
      tab[0]=2;
      tab[1]=4;
      break;
    case 5:
      tab[0]=3;
      tab[1]=4;
      break;
    case 6:
      tab[0]=3;
      tab[1]=1;
      break;
    case 7:
      tab[0]=4;
      tab[1]=1;
      break;
    case 8:
      tab[0]=4;
      tab[1]=4;
      break;
  }
}

void RobotsInterface::FaireTache(int num_poste, int duree)
{
  if(num_poste>=1 && num_poste<=nbRobot*2)
  {
    int tab[2];
    computeFromNumPoste(num_poste,tab);
    int num_robot=tab[0];
    int position=tab[1];
    if(position==1)
      robotTask[num_robot-1][0]=0;
    if(position==4)
      robotTask[num_robot-1][1]=0;
    tache_msg.num_robot=num_robot;
    tache_msg.position=position;
    tache_msg.duree=duree;
    pub_faireTache.publish(tache_msg);

    ros::Duration(1).sleep();
  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du poste doit etre compris entre 1 et "
         << nbRobot*2 << "." << RESET << endl;
}

void RobotsInterface::Evacuer()
{
  std_msgs::Byte msg;
  pub_evacuer_piece.publish(msg);

  // Pour eviter les conflits d'appel de service Coppelia en même temps
  ros::Duration(1).sleep();

  cout << BOLDCYAN << "Evacuation !" << RESET << endl;
}

void RobotsInterface::AjouterProduit(int poste, int produit)
{
  if(poste>=1 && poste<=nbRobot*2)
  {
    if(produit>=1 && produit<=6)
    {
      srv.request.choixPoste = poste;
      srv.request.choixProduit = produit;
      client.call(srv);

      // Pour le log
      msg0.num_poste = poste;
      //rappel, code produit A:14, B:24, C:34 etc.
      msg0.num_produit = produit*10+4;
      pubProductAdd.publish(msg0);

      produit_a_ajouter.push_back(poste*10+produit);
    }
    else
      cout <<  BOLDMAGENTA
           << "Le produit doit etre compris entre 1 et 6." << RESET << endl;

  }
  else
    cout <<  BOLDMAGENTA
         << "Le numero du poste doit etre compris entre 1 et "
         << nbRobot*2 << "." << RESET << endl;

}

bool RobotsInterface::ProductAddPushBack
(aip_cf_commande_locale::SrvAddProductPushBack::Request& req,
 aip_cf_commande_locale::SrvAddProductPushBack::Response& rep)
{
  int poste=req.poste;
  int produit=req.produit;
  produit_a_ajouter.push_back(poste*10+produit);
  return true;
}

int RobotsInterface::AjoutProduitEnAttente()
{
  int retour=-1;
  if(produit_a_ajouter.size()>0)
  {
    retour = produit_a_ajouter.back();
    produit_a_ajouter.pop_back();
  }
  return retour;
}
