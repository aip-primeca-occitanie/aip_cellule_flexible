#include <ros/ros.h>
#include <unistd.h>
#include <vector>
#include "Aiguillage.h"
#include <iostream>


using namespace std;

void ShutdownCallback(const std_msgs::Byte::ConstPtr& msg)
{
		ros::shutdown();
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "main_aiguillage");
	ros::NodeHandle noeud;
	ros::Subscriber sub_shutdown = noeud.subscribe("/commande_locale/shutdown",10,&ShutdownCallback);

	Aiguillage *gerer_aiguillage = new Aiguillage(noeud);

	ros::Rate loop_rate(25); //fréquence de la boucle

	while (ros::ok())
	{
		ros::spinOnce(); //permet aux fonction callback de ros dans les objets d'êtres appelées
		loop_rate.sleep(); //permet de synchroniser la boucle while. Il attend le temps qu'il reste pour faire le 25Hz (ou la fréquence indiquée dans le loop_rate)
	}

	delete gerer_aiguillage;

	return 0;
}
