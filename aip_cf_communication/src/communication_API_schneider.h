/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/

#ifndef Communication_API_sch
#define Communication_API_sch

#include <ros/ros.h>
#include <string>
#include <iostream>
#include <aip_cf_automates/Entrees.h>
#include <aip_cf_automates/Sorties.h>

class Communication_API_schneider
{
private:

	ros::Publisher pub_sorties;
	ros::Subscriber sub_entrees;


public:

	int entrees_api;
	int sorties_api;

	Communication_API_schneider(ros::NodeHandle noeud,
                                    const std::string num_API);
	~Communication_API_schneider();	
	void Callback_Entrees_api(const aip_cf_automates::Entrees msg);
	void publish();

};

uint16_t MASK(uint16_t registre,int voie);

#endif
