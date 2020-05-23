/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/

#include "communication_API_schneider.h"
#include "communication_commande.h"
#include <ros/ros.h>
#include <iostream>
#include "aip_cf_automates/Entrees.h"
#include "aip_cf_automates/Sorties.h"
#include "aip_cf_commande_locale/Actionneurs.h"
#include "aip_cf_commande_locale/Capteurs.h"

using namespace std;

////// Classe communication_API_schneider //////

// Constructeur 
Communication_API_schneider::Communication_API_schneider
(ros::NodeHandle noeud,const std::string num_API)
{
  // Publishers
  pub_sorties = noeud.advertise<aip_cf_automates::Sorties>
                ("/automates/Sorties_ap"+num_API, 1);

  // Subscriber
  sub_entrees = noeud.subscribe
                ("/automates/Entrees_ap"+num_API, 1,
                 &Communication_API_schneider::Callback_Entrees_api,this);


  entrees_api=0;
  sorties_api=0;
}

// Destructeur
Communication_API_schneider::~Communication_API_schneider()
{
}

// Recupération des entrées d'un API lorsqu'elles sont publiées
void Communication_API_schneider::Callback_Entrees_api
(const aip_cf_automates::Entrees msg)
{
  entrees_api=msg.entrees;
  ROS_INFO("API Entrees : %d", msg.entrees);
}

// Envoie des sorties d'un automate
void Communication_API_schneider::publish()
{
  aip_cf_automates::Sorties msg;
  msg.sorties=sorties_api;
  pub_sorties.publish(msg);
}



////// Classe Communication_commande //////

// Constructeur
Communication_commande::Communication_commande(ros::NodeHandle noeud)
{
  // Publishers
  pub_capteurs = noeud.advertise<aip_cf_commande_locale::Capteurs>
                 ("/commande/Ligne_transitique/Capteurs", 1);

  // Subscriber
  sub_actionneurs = noeud.subscribe
                    ("/commande/Ligne_transitique/Actionneurs", 1,
                     &Communication_commande::Callback_Actionneurs,this);


  CAPTEURS=0;
  ACTIONNEURS=0;

}

// Destructeur
Communication_commande::~Communication_commande()
{
}

//
void Communication_commande::Callback_Actionneurs
(const aip_cf_commande_locale::Actionneurs msg)
{
  ACTIONNEURS=msg.actionneurs;
  ROS_INFO("Actionneurs : %ld",msg.actionneurs);
}

//
void Communication_commande::publish()
{
  aip_cf_commande_locale::Capteurs msg;
  msg.capteurs=CAPTEURS;
  pub_capteurs.publish(msg);
}

void Communication_commande::Concatenation_entrees
(Communication_API_schneider AP1,Communication_API_schneider AP2)
{
  CAPTEURS = AP1.entrees_api + (((long int)AP2.entrees_api)<<16);
}

void Communication_commande::Decoupe_sorties
(Communication_API_schneider *AP1,Communication_API_schneider *AP2)
{
  AP1->sorties_api=ACTIONNEURS &(0xFFFF);
  AP2->sorties_api=(ACTIONNEURS &(0xFFFF0000))>>16;
}

////// Autres fonctions //////


uint16_t MASK(uint16_t registre,int voie)
{
  return (registre & (uint16_t)pow(2,voie))/pow(2,voie);
}

