#ifndef COMMANDE_SIMU
#define COMMANDE_SIMU

#include <ros/ros.h>
#include "actionneurs.h"
#include "capteurs.h"
#include "RobotsInterface.h"
#include <std_msgs/Bool.h>
#include <std_msgs/Int32.h>
#include <aip_cf_commande_locale/Msg_StopControl.h>
#include <aip_cf_commande_locale/Msg_AddProduct.h>
#include <aip_cf_commande_locale/SrvFinInit.h>
#include <iostream>
#include <string>
#include <cstdlib>

class Commande
{
	private:

	//Aiguillages
	ros::Subscriber SubDeverouilleAiguillages;
	ros::Subscriber SubVerouilleAiguillages;
	ros::Subscriber SubAiguillagesGauches;
	ros::Subscriber SubAiguillagesDroits;

	// Ergots
	//ros::Subscriber subPinOn;
	//ros::Subscriber subPinOff;

	ros::Subscriber sub_nouveau_produit;
	ros::Publisher pub_PetriTermine;

	ros::ServiceClient clientFinInit;
	aip_cf_commande_locale::SrvFinInit srvFinInit;

	// Actionneurs
	ros::Publisher pub_navettes_stops;
	ros::Publisher pub_actionneurs_simu_aiguillages;
  	ros::Publisher pub_actionneurs_simu_pins;

	ros::Subscriber sub_playSim;
	ros::Subscriber sub_pauseSim;

	bool play;
	int arrivee_produit;
	int poste;
	int produit;

	/* Capteurs */
	bool PSx[25],DxD[13],DxG[13],CPx[11],CPIx[9];

	/* Actionneurs */
	bool STx[25],RxD[13],RxG[13],Vx[13],Dx[13],PIx[9];

	//messages pour actionneurs
	aip_cf_commande_locale::Msg_StopControl actionneurs_simulation_Stop;
	aip_cf_commande_locale::Msg_SwitchControl
        actionneurs_simulation_Aiguillages;
	aip_cf_commande_locale::Msg_PinControl actionneurs_simulation_Pin;

public:

	Commande(ros::NodeHandle noeud, std::string executionPath);

	void Initialisation();

	void PlayCallback(const std_msgs::Byte::ConstPtr& msg);
	void PauseCallback(const std_msgs::Byte::ConstPtr& msg);
	bool getPlay();

  	void NouveauProduitCallback
        (const aip_cf_commande_locale::Msg_AddProduct::ConstPtr& msg);
	void Stop_PS(int point_stop);
	void Ouvrir_PS(int point_stop);

	void DeverouilleAiguillagesCallback(const std_msgs::Int32::ConstPtr& msg);
	void VerouilleAiguillagesCallback(const std_msgs::Int32::ConstPtr& msg);
	void AiguillagesgauchesCallback(const std_msgs::Int32::ConstPtr& msg);
	void AiguillagesdroitsCallback(const std_msgs::Int32::ConstPtr& msg);

	void SortirErgot(int num_ergot);
	void RentrerErgot(int num_ergot);
	int get_code_arrivee();
	int get_arrivee_nouveau_produit();
	void renitialiser_arrivee_nouveau_produit();

	void FinPetri();
};
#endif
