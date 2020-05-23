/**** Projet long N7 2017 ****/

#include "Robot.h"

void ShutdownCallback(const std_msgs::Byte::ConstPtr& msg)
{
	ros::shutdown();
}

int main(int argc, char **argv)
{
	//Initialisation du noeud ROS
	string name="robot";
	name.append(argv[1]);
	ros::init(argc,argv,name);
	ros::NodeHandle nh;
	ros::Subscriber sub_shutdown = nh.subscribe("/commande_locale/shutdown",10,&ShutdownCallback);

	//CmdRobot
	Robot robot(atoi(argv[1]));
	robot.init(nh);

	int compteur=0;
	ros::Rate loop_rate(25);
	while(ros::ok())
	{
		if(compteur++>25)
		{
			robot.update();
			compteur=0;
		}
		ros::spinOnce();
		loop_rate.sleep();


	}
}
