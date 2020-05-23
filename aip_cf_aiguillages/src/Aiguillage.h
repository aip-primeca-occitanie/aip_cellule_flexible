#ifndef AIGUILLAGE
#define AIGUILLAGE

#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include <unistd.h>
#include <std_msgs/Byte.h>


using namespace std;

class Aiguillage
{
private:
	ros::Subscriber VREPsubSwitchSensor;//Capteurs butée aiguillage
	ros::Subscriber sub_cmd_Droite;
	ros::Subscriber sub_cmd_Gauche;

	ros::Rate* loop_rate;

	ros::Publisher AigDev;
	ros::Publisher AigVer;
	ros::Publisher AigGauche;
	ros::Publisher AigDroite;

	bool Aig_D[12];//Capteurs butée
	bool Aig_G[12];

public:
	Aiguillage(ros::NodeHandle nh);
	~Aiguillage();

	void SwitchSensorCallback(const std_msgs::Int32::ConstPtr& msg);
	void GaucheCallback(const std_msgs::Int32::ConstPtr& msg_aigs);
	void DroiteCallback(const std_msgs::Int32::ConstPtr& msg_aigs);
};
#endif
