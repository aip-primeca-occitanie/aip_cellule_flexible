/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/

#include "API_schneider.h"
#include <ros/ros.h>
#include <modbus/modbus.h>
#include <iostream>
#include "aip_cf_automates/Entrees.h"
#include "aip_cf_automates/Sorties.h"

#define ADRESSE_ENTREES 1
#define ADRESSE_SORTIES 0
#define NOMBRE_REGISTRES 1


API_schneider::API_schneider(ros::NodeHandle noeud,
                             const std::string num_API,
                             const char* adresse_ip_api) 
{
  // Publishers
  pub_entrees = noeud.advertise<aip_cf_automates::Entrees>
                ("/automates/Entrees_ap"+num_API, 1);

  // Subscriber
  sub_sorties = noeud.subscribe("/automates/Sorties_ap"+num_API, 1,
                                &API_schneider::Callback,this);

  // Adresse IP
  adresse_ip = adresse_ip_api;
	
}

API_schneider::~API_schneider()
{
}


uint16_t API_schneider::get_register(int adresse_registre, int nombre_registres)
{
  modbus_t *ap;
  ap = modbus_new_tcp(adresse_ip,502);
  uint16_t registre;
    
  // On ouvre la liaison Modbus avec l'automate    
  modbus_connect(ap);

  // Si la fonction modbus_read_registers a fonctionné, on récupère les entrées
  // et sorties de l'automate et on les stocke dans le message créé plus haut

  if (modbus_read_registers(ap,adresse_registre,nombre_registres,&registre)
      > 0) 
  {
    ROS_INFO("Lecture reussie !");	
  }
  else ROS_INFO("Erreur lecture");
		
  // On ouvre la liaison Modbus avec l'automate
  modbus_close(ap);
  modbus_free(ap);

  return registre;
}

void API_schneider::write_register(int adresse_registre,
                                   int nombre_registres,uint16_t registre)
{
  modbus_t *ap;
  ap = modbus_new_tcp(adresse_ip,502);
    
  // On ouvre la liaison Modbus avec l'automate    
  modbus_connect(ap);

  // Si la fonction modbus_write_registers a fonctionné,
  // on renvoie "Ecriture reussie !"

  if (modbus_write_registers(ap,adresse_registre,nombre_registres,&registre)
      > 0) 
  {
    //ROS_INFO("Ecriture reussie !");	
  }
  else ROS_INFO("Erreur ecriture");
		
  // On ouvre la liaison Modbus avec l'automate
  modbus_close(ap);
  modbus_free(ap);
}

void API_schneider::Callback(const aip_cf_automates::Sorties msg)
{
  ROS_INFO("API [%s] Sorties : %d",adresse_ip,msg.sorties);
  sorties=msg.sorties;
  write_register(ADRESSE_SORTIES,NOMBRE_REGISTRES,sorties);
}



void API_schneider::publish()
{
  aip_cf_automates::Entrees msg;
  entrees=get_register(ADRESSE_ENTREES,NOMBRE_REGISTRES);
  msg.entrees=entrees;
  pub_entrees.publish(msg);
}








