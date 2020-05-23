/**** Bruno DATO M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/
#ifndef BAXT
#define BAXT

#include <ros/ros.h>
#include "std_msgs/Bool.h"

class Communication_Baxter
{
private:

	ros::Publisher pub_prise_demandee_bras_droit , pub_prise_demandee_bras_gauche ;

	ros::Subscriber sub_prise_effectuee_bras_droit , sub_prise_effectuee_bras_gauche, sub_attente_prise_bras_droit, sub_attente_prise_bras_gauche  ;

public:

	std_msgs::Bool msg_prise_demandee_bras_droit ,msg_prise_demandee_bras_gauche ;
	std_msgs::Bool msg_prise_effectuee_bras_droit,msg_prise_effectuee_bras_gauche ;
	std_msgs::Bool msg_attente_prise_bras_droit,msg_attente_prise_bras_gauche ;

	Communication_Baxter(ros::NodeHandle noeud);
	~Communication_Baxter();

	void Callback_prise_effectuee_bras_droit(const std_msgs::Bool& msg);
	void Callback_prise_effectuee_bras_gauche(const std_msgs::Bool& msg);
	void Callback_attente_prise_bras_gauche(const std_msgs::Bool& msg);
	void Callback_attente_prise_bras_droit(const std_msgs::Bool& msg);

	bool Prise_effectuee_bras_droit();
	bool Prise_effectuee_bras_gauche();

	bool Attente_prise_bras_droit();
	bool Attente_prise_bras_gauche();

	void Demander_prise_bras_droit();
	void Demander_prise_bras_gauche();

	void Afficher_Communication_Baxter();

	void Update();
};
#endif
