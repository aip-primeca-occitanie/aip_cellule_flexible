/*
 * ********************************* *
 * Copyright 2016, STEC Projet Long. *
 * All rights reserved.  	     *
 * ********************************* *
 * Mise à jour par le Projet Long    *
 * ENSEEIHT 2017		     *
 * ********************************* *
 */

#include "vrepController.h"

using namespace std;

// Initialisation du nombre de plateformes F
vrepController::vrepController(){
	repSim_startSimulation=false;
	repSim_pauseSimulation=false;
	repSim_loadModel=false;
	repSim_changeColor=false;
	repSim_getColor=false;
}

// PAUSE
void vrepController::pause()
{
	pubSim_pauseSimulation.publish(msgSim_pauseSimulation);
	while(!repSim_pauseSimulation&&ros::ok())
	{
		ros::spinOnce();
		loop_rate->sleep();
	}
	repSim_pauseSimulation = false;
}

// PLAY
void vrepController::play()
{
	pubSim_startSimulation.publish(msgSim_startSimulation);
	while(!repSim_startSimulation&&ros::ok())
	{
		ros::spinOnce();
		loop_rate->sleep();
	}
	repSim_startSimulation = false;
}

// Chargement des modèles dans la simulation lors de son lancement
void vrepController::loadModelInit(int shuttleNumber)
{
	char shuttleChar;
	if(shuttleNumber<0 || shuttleNumber>6) printf(" ATTENTION, LE NUMERO DU SHUTTLE DOIT ETRE COMPRIS ENTRE 0 ET 6 \n");
	else {
		if(shuttleNumber == 0) shuttleChar = (char)(90); // SI 0 -> ShuttleZ
		else shuttleChar = char(shuttleNumber+64);
		std::string shuttleName = "models/montrac/shuttle"+std::string(&shuttleChar)+".ttm";

		msgSim_loadModel.data = shuttleName;
		pubSim_loadModel.publish(msgSim_loadModel);
		while(!repSim_loadModel&&ros::ok())
		{
			ros::spinOnce();
			loop_rate->sleep();
		}
		repSim_loadModel = false;
	}
}

// On ferme le processus vrep
void vrepController::close()
{
	system("pkill vrep");
}

void vrepController::init(ros::NodeHandle n,std::string executionPath, std::string simulationFileName)
{
	loop_rate = new ros::Rate(25);
	// Launch of V-Rep
	int count = 0 ;
	int pos = executionPath.length()-1;
	while (count < 5 || pos<0)
	{
		if(executionPath[pos] == '/') count++;
		pos--;
	}
	if(pos<0) ROS_ERROR("pos negative !!!");

	std::string VRepPath = executionPath.substr(0,pos+2)+ "CoppeliaSim";

	char final_command[1000];
	sprintf(final_command, "cd %s &&(./coppeliaSim.sh -s0 -q ../sim/%s.ttt &)", VRepPath.c_str(), simulationFileName.c_str());
	system(final_command); // On execute VREP sans afficher la fenetre

	// Topic pour V-Rep
	pubSim_startSimulation = n.advertise<std_msgs::Byte>("/sim_ros_interface/services/vrep_controller/StartSimulation",100);
	subSim_startSimulation = n.subscribe("/sim_ros_interface/services/response/vrep_controller/StartSimulation",100,&vrepController::simStartSimulationCallback,this);

	pubSim_pauseSimulation = n.advertise<std_msgs::Byte>("/sim_ros_interface/services/vrep_controller/PauseSimulation",100);
	subSim_pauseSimulation = n.subscribe("/sim_ros_interface/services/response/vrep_controller/PauseSimulation",100,&vrepController::simPauseSimulationCallback,this);

	pubSim_loadModel = n.advertise<std_msgs::String>("/sim_ros_interface/services/vrep_controller/LoadModel",100);
	subSim_loadModel = n.subscribe("/sim_ros_interface/services/response/vrep_controller/LoadModel",100,&vrepController::simLoadModelCallback,this);

	pubSim_changeColor = n.advertise<std_msgs::Int32MultiArray>("/sim_ros_interface/services/vrep_controller/ChangeColor",100);
	subSim_changeColor = n.subscribe("/sim_ros_interface/services/response/vrep_controller/ChangeColor",100,&vrepController::simChangeColorCallback,this);

	pubSim_getColor = n.advertise<std_msgs::String>("/sim_ros_interface/services/vrep_controller/GetColor",100);
	subSim_getColor = n.subscribe("/sim_ros_interface/services/response/vrep_controller/GetColor",100,&vrepController::simGetColorCallback,this);

	pubStopTacheRobot1 = n.advertise<std_msgs::Int32>("/commande/Simulation/Robot1/StopTache",100);
	pubStopTacheRobot2 = n.advertise<std_msgs::Int32>("/commande/Simulation/Robot2/StopTache",100);
	pubStopTacheRobot3 = n.advertise<std_msgs::Int32>("/commande/Simulation/Robot3/StopTache",100);
	pubStopTacheRobot4 = n.advertise<std_msgs::Int32>("/commande/Simulation/Robot4/StopTache",100);


	pub_erreur_log = n.advertise<aip_cf_commande_locale::Msg_Erreur>
                         ("/commande/Simulation/Erreur_log",100);

	ros::Duration(1).sleep();
}

void vrepController::setSimulationFile(std::string fileName)
{
	this->SimulationFileName = fileName;
}

int vrepController::computeTableId(int poste)
{
	int id=-1;
	switch(poste)
	{
		case 1:
			id=0;
			break;
		case 2:
			id=1;
			break;
		case 3:
			id=3;
			break;
		case 4:
			id=4;
			break;
		case 5:
			id=6;
			break;
		case 6:
			id=7;
			break;
		case 7:
			id=9;
			break;
		case 8:
			id=10;
			break;
	}
	return id;
}

void vrepController::computeNumRobotPosteTache(int poste, int tab[2])
{
	// tab[0]=numRobot
	// tab[1]=posTache
	switch(poste)
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

void vrepController::addProduct(int produit, int poste)
{
	// On verif si on ecrase rien
	msgSim_getColor.data="Table#"+to_string(computeTableId(poste))+"#0_color";
	pubSim_getColor.publish(msgSim_getColor);
	while(!repSim_getColor && ros::ok())
	{
		ros::spinOnce();
		loop_rate->sleep();
	}
	repSim_getColor=false;
	int couleurLue=valueSim_getColor;

	if(couleurLue!=0)
	{
		ROS_ERROR("ERREUR : On ecrase un produit !!");
		aip_cf_commande_locale::Msg_Erreur msgErreur;
		msgErreur.code=66; // 66=code ecrasement de produit
		msgErreur.n_poste=poste;
		pub_erreur_log.publish(msgErreur);

		int tab[2]; // [0]=num_robot, [1]=posTache
		computeNumRobotPosteTache(poste,tab);
		std_msgs::Int32 msg;
		msg.data=tab[1];
		switch(tab[0])
		{
			case 1: //robot1
				pubStopTacheRobot1.publish(msg);
				break;

			case 2: //robot2
				pubStopTacheRobot2.publish(msg);
				break;

			case 3: //robot3
				pubStopTacheRobot3.publish(msg);
				break;

			case 4: //robot4
				pubStopTacheRobot4.publish(msg);
				break;
		}
	}

	// On ajoute le produit
	msgSim_changeColor.data.clear();
	msgSim_changeColor.data.push_back(computeTableId(poste));
	int couleur=produit*10+4;
	msgSim_changeColor.data.push_back(couleur);
	for(int i=0; i<NB_CUBE-1; i++)
		msgSim_changeColor.data.push_back(0);

	pubSim_changeColor.publish(msgSim_changeColor);
	while(!repSim_changeColor && ros::ok())
	{
		ros::spinOnce();
		loop_rate->sleep();
	}
	repSim_changeColor=false;
}

/** Callbacks pour V-Rep **/
void vrepController::simChangeColorCallback(const std_msgs::Byte::ConstPtr& msg)
{
	repSim_changeColor=true;
}

void vrepController::simStartSimulationCallback(const std_msgs::Byte::ConstPtr& msg)
{
	repSim_startSimulation=true;
}

void vrepController::simPauseSimulationCallback(const std_msgs::Byte::ConstPtr& msg)
{
	repSim_pauseSimulation=true;
}

void vrepController::simLoadModelCallback(const std_msgs::Int32::ConstPtr& msg)
{
	valueSim_loadModel=msg->data;
	repSim_loadModel=true;
}

void vrepController::simGetColorCallback(const std_msgs::Int32::ConstPtr& msg)
{
	valueSim_getColor=msg->data;
	repSim_getColor=true;
}
