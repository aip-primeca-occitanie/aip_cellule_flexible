/*
 * ********************************* *
 * Copyright 2016, STEC Projet Long. *
 * All rights reserved.  	     *
 * ********************************* *
 * Mise à jour par le Projet Long    *
 * ENSEEIHT 2017		     *
 * ********************************* *
*/

#ifndef VREP_CONTROLLER
#define VREP_CONTROLLER

#include <ros/ros.h>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <std_msgs/Byte.h>
#include <std_msgs/Int32MultiArray.h>
#include <aip_cf_shuttles/msgShuttleChange.h>
#include <aip_cf_commande_locale/Msg_Erreur.h>

#include "aip_cf_commande_locale/Msg_Color.h"

#include <aip_cf_aiguillages/ExchangeSh.h>

// Cf la documentation paragraphe 4
// pour modifier le nombre de cube max
#define NB_CUBE 6

class vrepController
{
	private:
		//** Topic V-Rep **//

		// StartSimulation
		ros::Publisher pubSim_startSimulation;
		std_msgs::Byte msgSim_startSimulation;
		ros::Subscriber subSim_startSimulation;
		bool repSim_startSimulation;

		// PauseSimulation
		ros::Publisher pubSim_pauseSimulation;
		std_msgs::Byte msgSim_pauseSimulation;
		ros::Subscriber subSim_pauseSimulation;
		bool repSim_pauseSimulation;

		// LoadModel
		ros::Publisher pubSim_loadModel;
		std_msgs::String msgSim_loadModel;
		ros::Subscriber subSim_loadModel;
		bool repSim_loadModel;
		int valueSim_loadModel;

		// RemoveModel
		ros::Publisher pubSim_removeModel;
		std_msgs::Int32 msgSim_removeModel;
		ros::Subscriber subSim_removeModel;
		bool repSim_removeModel;
		int valueSim_removeModel;

		// ChangeColor
		ros::Publisher pubSim_changeColor;
		std_msgs::Int32MultiArray msgSim_changeColor;
		ros::Subscriber subSim_changeColor;
		bool repSim_changeColor;

		// GetColor
		ros::Publisher pubSim_getColor;
		std_msgs::String msgSim_getColor;
		ros::Subscriber subSim_getColor;
		bool repSim_getColor;
		int valueSim_getColor;

		// Gestion de navettes
		ros::Subscriber DeleteShuttle;
		ros::Publisher createShuttle;

		ros::Publisher pub_erreur_log;
		ros::Publisher pubStopTacheRobot1;
		ros::Publisher pubStopTacheRobot2;
		ros::Publisher pubStopTacheRobot3;
		ros::Publisher pubStopTacheRobot4;

		std::string SimulationFileName;

		ros::Rate* loop_rate;
	public:
		vrepController ();
		void init(ros::NodeHandle n,std::string executionPath,
                          std::string simulationFileName);
		void close();
		void loadModelInit(int shuttleNumber);
		//void removeModel(int32_t handle);
		//int32_t getObjectHandle(std::string);
		void waitVrep();
		void play();
		void pause();

		void setSimulationFile(std::string);
		void addNewShuttle
                (int handle_navette, int handle_plateforme,
                 int type, int destination);
		void ColorCallBack
                (const aip_cf_commande_locale::Msg_Color::ConstPtr& msg);

		int computeTableId(int poste);
		void addProduct(int produit, int poste);

		void computeNumRobotPosteTache(int poste,int tab[2]);

		// Callbacks pour V-Rep
		void simChangeColorCallback
                (const std_msgs::Byte::ConstPtr& msg);
		void simStartSimulationCallback
                (const std_msgs::Byte::ConstPtr& msg);
		void simPauseSimulationCallback
                (const std_msgs::Byte::ConstPtr& msg);
		void simLoadModelCallback(const std_msgs::Int32::ConstPtr& msg);
		void simGetColorCallback(const std_msgs::Int32::ConstPtr& msg);
};

#endif
