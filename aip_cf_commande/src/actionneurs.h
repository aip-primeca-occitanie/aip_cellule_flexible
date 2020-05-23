/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/
#ifndef ACT
#define ACT

#include <aip_cf_commande_locale/Msg_StopControl.h>
#include "aip_cf_commande_locale/Msg_SwitchControl.h"
#include "aip_cf_commande_locale/Msg_PinControl.h"
#include "aip_cf_commande_locale/Actionneurs.h"
#include <ros/ros.h>


class Actionneurs
{
private:

	ros::Publisher pub_actionneurs_ligne,pub_actionneurs_simu_aguillages,pub_actionneurs_simu_stops,pub_actionneurs_simu_pins;

public:
	
	long int Actionneurs_ligne;
	aip_cf_commande_locale::Msg_StopControl actionneurs_simulation_Stop;
	aip_cf_commande_locale::Msg_SwitchControl
        actionneurs_simulation_Aguillages;
	aip_cf_commande_locale::Msg_PinControl actionneurs_simulation_Pin;

	Actionneurs(ros::NodeHandle noeud);
	~Actionneurs();

	void Envoyer(bool STx[],bool RxD[],bool RxG[],bool Vx[],bool Dx[],bool PIx[]);

	void publish_actionneurs_ligne();
	void publish_actionneurs_simulation();

	void Ecrire_ligne_STx(bool STx[]);
	void Ecrire_ligne_RxD(bool RxD[]);
	void Ecrire_ligne_RxG(bool RxG[]);
	void Ecrire_ligne_PIx(bool PIx[]);
	void Ecrire_ligne_Vx(bool Vx[]);
	void Ecrire_ligne_Dx(bool Dx[]);

};

void WRITE(long int *registre,bool donnee,int numero_bit);

#endif
