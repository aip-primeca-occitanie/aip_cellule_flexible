#ifndef AIGSINTERFACE
#define AIGSINTERFACE

#include <ros/ros.h>
#include <iostream>
//Messages
#include <std_msgs/Int32.h>


class AigsInterface
{
private:
	ros::Publisher pub_aig_Droite;
	ros::Publisher pub_aig_Gauche;

	std_msgs::Int32 msg;
	std_msgs::Int32 msg_aigs;

public:
	AigsInterface(ros::NodeHandle noeud);
	~AigsInterface();

	//Contrôles des aiguillages
	void Droite(int num_Aig);
	void Gauche(int num_Aig);
};
#endif
