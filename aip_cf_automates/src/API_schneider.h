/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/

#ifndef API_sch
#define API_sch

#include <ros/ros.h>
#include <string>
#include <iostream>
#include "aip_cf_automates/Entrees.h"
#include "aip_cf_automates/Sorties.h"

class API_schneider
{
 private:
  
  ros::Publisher pub_entrees;
  ros::Subscriber sub_sorties;
  const char* adresse_ip;
  uint16_t entrees,sorties;
  
public:

	
  API_schneider(ros::NodeHandle noeud,const std::string num_API,const char* adresse_ip_api) ;
  ~API_schneider();	
  uint16_t get_register(int adresse_registre, int nombre_registres);
  void write_register(int adresse_registre, int nombre_registres,uint16_t registre);
  void Callback(const aip_cf_automates::Sorties msg);
  void publish();

};


#endif
