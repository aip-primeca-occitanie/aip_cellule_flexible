/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/

#include "API_schneider.h"
#include <ros/ros.h>

#define ADRESSE_IP_API1 "192.168.0.101"
#define ADRESSE_IP_API2 "192.168.0.102" 
#define ADRESSE_IP_API3 "192.168.0.103"
#define ADRESSE_IP_API4 "192.168.0.104"
#define ADRESSE_IP_API5 "192.168.0.105" 

int main(int argc, char **argv)
{

	ros::init(argc, argv, "automates");
	ros::NodeHandle noeud;

	API_schneider AP1(noeud,"1",ADRESSE_IP_API1),AP2(noeud,"2",ADRESSE_IP_API2);


	ros::Rate loop_rate(30);

	
	while (ros::ok()) // Tant que ROS est actif,
	{
		AP1.publish();
		AP2.publish();

		ros::spinOnce(); // on lance les callback correspondant aux messages entrants.

		loop_rate.sleep();
	}

	return 0;
}
