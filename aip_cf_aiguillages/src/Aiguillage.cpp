#include "Aiguillage.h"

using namespace std;

Aiguillage::Aiguillage(ros::NodeHandle nh)
{
	loop_rate=new ros::Rate(25);

	VREPsubSwitchSensor = nh.subscribe("sim_ros_interface/SwitchSensor", 1000, &Aiguillage::SwitchSensorCallback, this);//Info sur position aiguillage
	sub_cmd_Droite = nh.subscribe("/commande/Simulation/AiguillageDroite",1000,&Aiguillage::DroiteCallback,this);
	sub_cmd_Gauche = nh.subscribe("/commande/Simulation/AiguillageGauche",1000,&Aiguillage::GaucheCallback,this);

	AigDev = nh.advertise<std_msgs::Int32>("/commande/DeverouilleAiguillage", 1000);
	AigVer = nh.advertise<std_msgs::Int32>("/commande/VerouilleAiguillage", 1000);
	AigGauche = nh.advertise<std_msgs::Int32>("/commande/AiguillageGauche", 1000);
	AigDroite = nh.advertise<std_msgs::Int32>("/commande/AiguillageDroite", 1000);

	usleep(1000000);
}

Aiguillage::~Aiguillage()
{
	delete loop_rate;
}

void Aiguillage::SwitchSensorCallback(const std_msgs::Int32::ConstPtr& msg)
{
	//mise a jour des tableaux sur les capteurs de butée des aiguillages
	//masque pour isoler les bits 2*i-1 et 2*i-2
	for(int i=1;i<13;i++)
	{
		Aig_D[i] = (msg->data & (int32_t)pow(2,2*i-2)) > 0;
		Aig_G[i] = (msg->data & (int32_t)pow(2,2*i-1)) > 0;
	}
}

void Aiguillage::GaucheCallback(const std_msgs::Int32::ConstPtr& msg_aigs)
{
	ROS_INFO("On bouge a gauche, aig numero %d", msg_aigs->data);
	if(!Aig_G[msg_aigs->data])
	{
		//Deverouillage de l'aiguillage
		AigDev.publish(msg_aigs);
		//Envoie ordre mouvement à gauche
		AigGauche.publish(msg_aigs);

		//Attente...
		while(!Aig_G[msg_aigs->data])
		{
			ros::spinOnce();
			if(Aig_D[msg_aigs->data]) AigGauche.publish(msg_aigs);
			loop_rate->sleep();
		}

		//Verouillage de l'aiguillage
		AigVer.publish(msg_aigs);
	}
}

void Aiguillage::DroiteCallback(const std_msgs::Int32::ConstPtr& msg_aigs)
{
	ROS_INFO("On bouge a droite, aig numero %d", msg_aigs->data);
	if(!Aig_D[msg_aigs->data])
	{
		//Deverouillage de l'aiguillage
		AigDev.publish(msg_aigs);
		//Envoie ordre mouvement à droite
		AigDroite.publish(msg_aigs);

		//Attente...
		while(!Aig_D[msg_aigs->data])
		{
			ros::spinOnce();
			if(Aig_G[msg_aigs->data]) AigDroite.publish(msg_aigs);
			loop_rate->sleep();
		}

		//Verouillage de l'aiguillage
		AigVer.publish(msg_aigs);
	}
}
