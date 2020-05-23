#include <ros/ros.h>
#include <ros/package.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Byte.h>
#include <std_msgs/Float32.h>
#include "aip_cf_commande_locale/Msg_AddProduct.h"
#include "aip_cf_commande_locale/Msg_Erreur.h"
#include "aip_cf_commande_locale/TacheFinieMsg.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

ros::Publisher pubSim_getTime;
ros::Subscriber subSim_getTime;
bool repSim_getTime;
float valueSim_getTime;
time_t tmm = time(0);
char* dt = ctime(&tmm);

string path=ros::package::getPath("commande_locale");
string filepath=path.substr(0,path.length()-(15+11))+"log.txt"; // 15="commande_locale.length 11="ros_ws/src/".length

ofstream monFlux(filepath, ios::app);  //On essaye d'ouvrir le fichier

void ProduitEvacCallback(const std_msgs::Int32MultiArray::ConstPtr& msg)
{
	ros::Rate loop_rate(25);
	pubSim_getTime.publish(std_msgs::Byte());
	while(!repSim_getTime && ros::ok())
	{
		ros::spinOnce();
		loop_rate.sleep();
	}
	repSim_getTime=false;
	float temps=valueSim_getTime;

	int couleur[4];
	monFlux<<"Sortie";
	for (int i=0;i<4;i++)
	{
		couleur[i]=msg->data[i];
		monFlux<<" :" <<couleur[i];
	}
	monFlux<<fixed<<setprecision(2)<<" :"<<temps;
	monFlux<<endl;
}

void NewProductCallback(const aip_cf_commande_locale::Msg_AddProduct::ConstPtr& msg)
{
	ros::Rate loop_rate(25);
	pubSim_getTime.publish(std_msgs::Byte());
	while(!repSim_getTime && ros::ok())
	{
		ros::spinOnce();
		loop_rate.sleep();
	}
	repSim_getTime=false;
	float temps=valueSim_getTime;

	monFlux<<"NouveauProduit: ";
	monFlux<<msg->num_produit;
	monFlux<<fixed<<setprecision(2)<<" :"<<temps;
	monFlux<<endl;
}

void ErreurCallback(const aip_cf_commande_locale::Msg_Erreur::ConstPtr& msg)
{
	switch(msg->code)
	{
		// Operation sur un poste vide
		case 1:
			monFlux<<"OperationPosteVide: ";
			monFlux<<msg->n_poste;
			monFlux<<endl;

			//a supprimer plus tard
			ROS_INFO("ERREUR poste Vide ide ide");
			ROS_INFO("	sur le poste: %d",msg->n_poste);
			break;

		// Operation sur un produit plein
		case 2:
			monFlux<<"OperationProduitPlein: ";
			monFlux<<msg->n_poste;
			monFlux<<endl;

			//a supprimer plus tard
			ROS_INFO("ERREUR Operation sur un produit plein");
			ROS_INFO("	sur le poste: %d",msg->n_poste);
			break;

		// Manipulation d'un produit en cours de traitement
		case 3:
			monFlux<<"ManipulationEnTraitement: ";
			monFlux<<msg->n_poste;
			monFlux<<endl;

			//a supprimer plus tard
			ROS_INFO("ERREUR Manipulation produit en traitement");
			ROS_INFO("	sur le poste: %d",msg->n_poste);
			break;

		// Manipulation d'un produit en cours de traitement
		case 4:
			monFlux<<"PerteNavette: ";
			monFlux<<msg->n_poste; // Correspond à la file
			monFlux<<endl;

			//a supprimer plus tard
			ROS_INFO("ERREUR Perte navette");
			ROS_INFO("	sur la file: %d",msg->n_poste);
			break;

		// Ecrasement d'un produit (ajout produit ou deplacement)
		case 66:
			monFlux<<"EcrasementProduit:";
			monFlux<<msg->n_poste; // num du robot
			monFlux<<endl;

			//a supprimer plus tard
			ROS_INFO("ERREUR On a ecrase un produit : c'est mal");
			ROS_INFO("	au robot %d", msg->n_poste);
			break;
	}
}

void TachefinieCallback(const aip_cf_commande_locale::TacheFinieMsg::ConstPtr& msg)
{
	monFlux<<"TempoT";
	monFlux<<": "<<msg->num_produit;
	monFlux<<": "<<msg->num_poste;
	monFlux<<": "<<msg->duree;
	monFlux<<endl;
}

void getTimeCallback(const std_msgs::Float32::ConstPtr& msg)
{
	valueSim_getTime=msg->data;
	repSim_getTime=true;
}

void ShutdownCallback(const std_msgs::Byte::ConstPtr& msg)
{
	ros::shutdown();
}

void PetriTermineCallback(const std_msgs::Byte::ConstPtr& msg)
{
	monFlux<<endl<<"Petri terminé" << endl;
}

int main(int argc, char **argv)
{
	ofstream monFlux2(filepath);
	monFlux2.close();

	ros::init(argc, argv, "log_manager");
	ros::NodeHandle nh;

	ros::Subscriber subTacheFinie = nh.subscribe("/commande/Simulation/TacheFinie", 1, &TachefinieCallback);
	ros::Subscriber subProduitEvac = nh.subscribe("/commande/Simulation/produitEvac", 1, &ProduitEvacCallback);
	ros::Subscriber subNewProduit = nh.subscribe("/commande_locale/AddProduct", 1, &NewProductCallback);
	ros::Subscriber subErreur = nh.subscribe("/commande/Simulation/Erreur_log", 1, &ErreurCallback);
	ros::Subscriber sub_shutdown = nh.subscribe("/commande_locale/shutdown",10,&ShutdownCallback);
	ros::Subscriber sub_terminee = nh.subscribe("/commande/PetriTermine",10,&PetriTermineCallback);

	// GetTime VREP
	pubSim_getTime=nh.advertise<std_msgs::Byte>("/sim_ros_interface/services/LogManager/GetTime",100);
	subSim_getTime=nh.subscribe("/sim_ros_interface/services/response/LogManager/GetTime",100,getTimeCallback);
	repSim_getTime=false;

	ros::Duration(1).sleep();

	ros::Rate loop_rate(25); //fréquence de la boucle

	ROS_INFO("LogManager initialise\n");
	monFlux << "Creation fichier log: " << dt << endl;

	while (ros::ok())
	{
		ros::spinOnce(); //permet aux fonction callback de ros dans les objets d'êtres appelées
		loop_rate.sleep(); //permet de synchroniser la boucle while. Il attend le temps qu'il reste pour faire le 25Hz (ou la fréquence indiquée dans le loop_rate)
	}

	return 0;
}
