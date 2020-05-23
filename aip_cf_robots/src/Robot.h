#ifndef ROBOT
#define ROBOT

#include <ros/ros.h>

//Messages
#include <aip_cf_commande_locale/RobotJoints.h>
#include <aip_cf_robots/MoveRobot.h>
#include <aip_cf_robots/FaireTacheMsg.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Byte.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <sensor_msgs/JointState.h>
#include <aip_cf_robots/Msg_numrobot.h>
#include <aip_cf_commande_locale/TacheFinieMsg.h>
#include <aip_cf_commande_locale/Msg_Erreur.h>
#include "aip_cf_commande_locale/DeplacerPieceMsg.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "Poste.h"
#include "aip_cf_shuttles/shuttle_id.h"

// Nombre de cube max sur les postes et navettes
//#define NB_CUBE 6
#include "../../aip_cf_commande_locale/src/vrepController.h"


class Robot
{
private:

	/** Topic V-Rep **/
	// Le type Byte est utilisé lorsque le contenu du message n'est pas utile

	// GetObjectHandle
	ros::Publisher pubSim_getObjectHandle;
	std_msgs::String msgSim_getObjectHandle;
	ros::Subscriber subSim_getObjectHandle;
	bool repSim_getObjectHandle;
	int valueSim_getObjectHandle;

	// SetJointState
	ros::Publisher pubSim_setJointState;
	std_msgs::Float32MultiArray msgSim_setJointState;
	ros::Subscriber subSim_setJointState;
	bool repSim_setJointState;

	// GetJointState
	ros::Publisher pubSim_getJointState;
	std_msgs::Int32 msgSim_getJointState;
	ros::Subscriber subSim_getJointState;
	bool repSim_getJointState;
	float valueSim_getJointState;

	// GetTime
	ros::Publisher pubSim_getTime;
	std_msgs::Byte msgSim_getTime;
	ros::Subscriber subSim_getTime;
	bool repSim_getTime;
	float valueSim_getTime;

	// GetTimeUpdate
	ros::Publisher pubSim_getTimeUpdate;
	std_msgs::Byte msgSim_getTimeUpdate;
	ros::Subscriber subSim_getTimeUpdate;
	bool repSim_getTimeUpdate;
	float valueSim_getTimeUpdate;

	// ChangeColor
	ros::Publisher pubSim_changeColor;
	std_msgs::Int32MultiArray msgSim_changeColor;
	ros::Subscriber subSim_changeColor;
	bool repSim_changeColor;

	// ChangeShuttleColor
	ros::Publisher pubSim_changeShuttleColor;
	std_msgs::Int32MultiArray msgSim_changeShuttleColor;
	ros::Subscriber subSim_changeShuttleColor;
	bool repSim_changeShuttleColor;

	// GetColor
	ros::Publisher pubSim_getColor;
	std_msgs::String msgSim_getColor;
	ros::Subscriber subSim_getColor;
	bool repSim_getColor;
	int valueSim_getColor;

	// GetColorUpdate
	ros::Publisher pubSim_getColorUpdate;
	std_msgs::String msgSim_getColorUpdate;
	ros::Subscriber subSim_getColorUpdate;
	bool repSim_getColorUpdate;
	int valueSim_getColorUpdate;

	/** Subscribers aux topics de la commande **/
	ros::Subscriber planifSendPosition;
	ros::Subscriber planifSendJoints;
	ros::Subscriber planifFermerPince;
	ros::Subscriber planifOuvrirPince;
	ros::Subscriber planifDescendreBras;
	ros::Subscriber planifMonterBras;
	ros::Subscriber planifControlerRobot;
	ros::Subscriber sub_colorer;
	ros::Subscriber sub_faireTache;
	ros::Subscriber sub_evacuer;
	ros::Subscriber subStopTache;
	ros::Subscriber subDeplacerPiece;

	/** Publishers pour retours **/
	ros::Publisher pub_pince, pub_robotPosition, pub_robotBras, pub_robotPince;
	ros::Publisher pub_retourCommande;

	ros::Publisher pub_robot_transport;
	ros::Publisher pub_produitEvac;

	ros::Publisher pub_erreur_log;
	ros::Publisher pub_tache_finie;

	aip_cf_commande_locale::TacheFinieMsg msg_tache_finie;

	ros::ServiceClient client;
	aip_cf_shuttles::shuttle_id srv;

	/** Variables **/
	float pi;
	unsigned char mymodes[7];
	float Rpos[7];
	int Rints[7];
	int couleur_transportee[NB_CUBE];
	// numero du robot
	int num_robot;
	ros::Rate* loop_rate;
	ros::Rate* loop_ok;

	/** Messages **/
	std_msgs::Int32 robotPosition;
	std_msgs::Int32 robotBras;
	std_msgs::Int32 robotPince;
	aip_cf_robots::Msg_numrobot retour;
	aip_cf_commande_locale::Msg_Erreur msg_erreur;


	std_msgs::Int32MultiArray msg_log_couleur;

	int produit_sur_poste;
	Poste poste_pos_1;
	Poste poste_pos_4;


public:

	Robot(int num_du_robot);
	~Robot();


	//Initialisation
	void init(ros::NodeHandle noeud);

	int computeTableId(int position);
	void transport(bool valeur);
	void update();

	/** Fonctions internes permettant le contrôle du robot **/
	//Pour atteindre une position prédéfinie
	void EnvoyerRobot(int position);

	//Pour atteindre une position définie manuellement
	void EnvoyerJoints(int joint1, int joint2, int joint3, int joint4, int joint5, int joint6, int joint7);

	//Pour monter ou descendre le bras
	void DescendreBras();
	void MonterBras();

	//Pour ouvrir ou fermer la pince
	void FermerPince();
	void OuvrirPince();

	/** Fonctions permettant de controler le robot avec des ordres du noeud commande **/
	//Pour atteindre une position prédéfinie
	void SendPositionCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg);

	//Pour atteindre une position définie manuellement
	void SendJointsCallback(const aip_cf_commande_locale::RobotJoints::ConstPtr& msg);

	//Pour monter ou descendre le bras
	void DescendreBrasCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg);
	void MonterBrasCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg);

	//Pour ouvrir ou fermer la pince
	void FermerPinceCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg);
	void OuvrirPinceCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg);

	//Pour contrôler l'ensemble des mouvements du robot
	void ControlerRobotCallback(const aip_cf_robots::MoveRobot::ConstPtr& msg);
	void Colorer(int position, int type);
	void faireTacheCallback(const aip_cf_robots::FaireTacheMsg::ConstPtr& msg);
	void ajouter_produitCallback(aip_cf_commande_locale::Msg_AddProduct msg);

	int colorerPosteDebutTask(int positionPoste);
	int colorerPosteFinTask(int positionPoste, int duree);
	void Evacuer(const std_msgs::Byte::ConstPtr& msg);

	void stopTacheCallback(const std_msgs::Int32::ConstPtr& msg);
	void DeplacerPieceCallback(const aip_cf_commande_locale::DeplacerPieceMsg::ConstPtr& msg);

	// Callbacks pour V-Rep
	void simGetObjectHandleCallback(const std_msgs::Int32::ConstPtr& msg);
	void simSetJointStateCallback(const std_msgs::Byte::ConstPtr& msg);
	void simGetJointStateCallback(const sensor_msgs::JointState::ConstPtr& msg);
	void simGetTimeCallback(const std_msgs::Float32::ConstPtr& msg);
	void simGetTimeUpdateCallback(const std_msgs::Float32::ConstPtr& msg);
	void simChangeColorCallback(const std_msgs::Byte::ConstPtr& msg);
	void simChangeShuttleColorCallback(const std_msgs::Byte::ConstPtr& msg);
	void simGetColorCallback(const std_msgs::Int32::ConstPtr& msg);
	void simGetColorUpdateCallback(const std_msgs::Int32::ConstPtr& msg);
};


#endif
