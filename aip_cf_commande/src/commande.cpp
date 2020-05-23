#include "commande.h"
#include <ros/ros.h>
#include <iostream>
#include <std_msgs/Int32.h>

using namespace std;

//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

Commande::Commande(ros::NodeHandle noeud, std::string executionPath)
{
	// Actionner ergots
	//subPinOn = noeud.subscribe("/Poste_Cmde/SortirErgots", 10, &Commande::SortirErgotCallback, this);
	//subPinOff = noeud.subscribe("/Poste_Cmde/RentrerErgots", 10, &Commande::RentrerErgotCallback, this);

	//Commande aiguillages
	SubDeverouilleAiguillages = noeud.subscribe("/commande/DeverouilleAiguillage", 100, &Commande::DeverouilleAiguillagesCallback, this);
	SubVerouilleAiguillages = noeud.subscribe("/commande/VerouilleAiguillage", 100, &Commande::VerouilleAiguillagesCallback, this);
	SubAiguillagesGauches = noeud.subscribe("/commande/AiguillageGauche", 100, &Commande::AiguillagesgauchesCallback, this);
	SubAiguillagesDroits = noeud.subscribe("/commande/AiguillageDroite", 100, &Commande::AiguillagesdroitsCallback, this);

	sub_pauseSim = noeud.subscribe("/sim_ros_interface/services/vrep_controller/PauseSimulation",10,&Commande::PauseCallback,this);
	sub_playSim = noeud.subscribe("/sim_ros_interface/services/vrep_controller/StartSimulation",10,&Commande::PlayCallback,this);
	play=false;
	clientFinInit = noeud.serviceClient<aip_cf_commande_locale::SrvFinInit>("srv_fin_init");
	pub_PetriTermine = noeud.advertise<std_msgs::Byte>("/commande/PetriTermine",10);

	// Publishers messages actionneurs
	pub_navettes_stops = noeud.advertise<aip_cf_commande_locale::Msg_StopControl>("/commande/Simulation/Actionneurs_stops", 100);
	pub_actionneurs_simu_aiguillages = noeud.advertise<aip_cf_commande_locale::Msg_SwitchControl>("/commande/Simulation/Actionneurs_aiguillages", 100);
	pub_actionneurs_simu_pins = noeud.advertise<aip_cf_commande_locale::Msg_PinControl>("/commande/Simulation/Actionneurs_pins", 100);

	ros::Duration(1).sleep();

	// Initialisation des Actionneurs
	for(int i=1;i<=24;i++) actionneurs_simulation_Stop.STOP[i] = 0;
	for(int i=1;i<=24;i++) actionneurs_simulation_Stop.GO[i] = 1;

	for(int i=1;i<=12;i++) actionneurs_simulation_Aiguillages.LOCK[i] = 0;
	for(int i=1;i<=12;i++) actionneurs_simulation_Aiguillages.RD[i] = 0;
	for(int i=1;i<=12;i++) actionneurs_simulation_Aiguillages.RG[i] = 0;

	for(int i=1;i<=8;i++) actionneurs_simulation_Pin.PINON[i] = 0;
	for(int i=1;i<=8;i++) actionneurs_simulation_Pin.PINOFF[i] = 1;
}

void Commande::Initialisation()
{
	for(int i=0;i<25;i++) PSx[i]=0;
	for(int i=0;i<13;i++) DxD[i]=0;
	for(int i=0;i<13;i++) DxG[i]=0;
	for(int i=0;i<11;i++) CPx[i]=0;
	for(int i=0;i<9;i++) CPIx[i]=0;

	for(int i=0;i<25;i++) STx[i]=1;
	for(int i=0;i<13;i++) RxD[i]=0;
	for(int i=0;i<13;i++) RxG[i]=0;

	for(int i=0;i<13;i++) Vx[i]=0;
	for(int i=0;i<13;i++) Dx[i]=0;
	for(int i=0;i<9;i++) PIx[i]=0;

	clientFinInit.call(srvFinInit);
}

void Commande::PlayCallback(const std_msgs::Byte::ConstPtr& msg)
{
	play=true;
}

void Commande::PauseCallback(const std_msgs::Byte::ConstPtr& msg)
{
	play=false;
}

bool Commande::getPlay()
{
	return play;
}


void Commande::Stop_PS(int point_stop)
{
	actionneurs_simulation_Stop.STOP[point_stop]=1;
	actionneurs_simulation_Stop.GO[point_stop]=0;
	pub_navettes_stops.publish(actionneurs_simulation_Stop);
}

void Commande::Ouvrir_PS(int point_stop)
{
	actionneurs_simulation_Stop.STOP[point_stop]=0;
	actionneurs_simulation_Stop.GO[point_stop]=1;
	pub_navettes_stops.publish(actionneurs_simulation_Stop);
}

void Commande::DeverouilleAiguillagesCallback(const std_msgs::Int32::ConstPtr& msg)
{
	Dx[msg->data]=1;
	Vx[msg->data]=0;
}

void Commande::VerouilleAiguillagesCallback(const std_msgs::Int32::ConstPtr& msg)
{
	Dx[msg->data]=0;
	Vx[msg->data]=1;
}

void Commande::AiguillagesgauchesCallback(const std_msgs::Int32::ConstPtr& msg)
{
	RxG[msg->data]=1;
	for(int i=0;i<13;i++) actionneurs_simulation_Aiguillages.RG[i] = RxG[i];
	for(int i=0;i<13;i++) actionneurs_simulation_Aiguillages.RD[i] = RxD[i];
	for(int i=0;i<13;i++) actionneurs_simulation_Aiguillages.LOCK[i] = !Vx[i] && Dx[i];
	pub_actionneurs_simu_aiguillages.publish(actionneurs_simulation_Aiguillages);
	RxG[msg->data]=0;
}

void Commande::AiguillagesdroitsCallback(const std_msgs::Int32::ConstPtr& msg)
{
	RxD[msg->data]=1;
	for(int i=0;i<13;i++) actionneurs_simulation_Aiguillages.RD[i] = RxD[i];
	for(int i=0;i<13;i++) actionneurs_simulation_Aiguillages.RG[i] = RxG[i];
	for(int i=0;i<13;i++) actionneurs_simulation_Aiguillages.LOCK[i] = !Vx[i] && Dx[i];
	pub_actionneurs_simu_aiguillages.publish(actionneurs_simulation_Aiguillages);
	RxD[msg->data]=0;
}

void Commande::SortirErgot(int num_ergot)
{
	PIx[num_ergot]=1;
	for(int i=1;i<=8;i++) actionneurs_simulation_Pin.PINON[i] = PIx[i];
	for(int i=1;i<=8;i++) actionneurs_simulation_Pin.PINOFF[i] = !PIx[i];
	pub_actionneurs_simu_pins.publish(actionneurs_simulation_Pin);
}

void Commande::RentrerErgot(int num_ergot)
{
	PIx[num_ergot]=0;
	for(int i=1;i<=8;i++) actionneurs_simulation_Pin.PINON[i] = PIx[i];
	for(int i=1;i<=8;i++) actionneurs_simulation_Pin.PINOFF[i] = !PIx[i];
	pub_actionneurs_simu_pins.publish(actionneurs_simulation_Pin);
}

void Commande::FinPetri()
{
	pub_PetriTermine.publish(std_msgs::Byte());
}
