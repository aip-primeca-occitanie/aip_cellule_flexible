/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/
#ifndef CAPT
#define CAPT

#include <aip_cf_aiguillages/Msg_SensorState.h>
#include <ros/ros.h>

class Capteurs
{
 private:
  ros::Subscriber sub_capteurs_ligne;
  ros::Subscriber sub_capteurs_simu;

  bool PSx[25],DxD[13],DxG[13],CPx[11],CPIx[9];

 public:
  Capteurs(ros::NodeHandle noeud);
  ~Capteurs();

  void Actualiser(bool PS[],bool DD[],bool DG[],bool CP[],bool CPI[]);

  void Callback_capteurs_simulation
  (const aip_cf_aiguillages::Msg_SensorState::ConstPtr& msg);

  bool get_CP(int num_CP);
  bool get_CPI(int num_CPI);
  bool get_DG(int num_DG);
  bool get_DD(int num_DD);
  bool get_PS(int num_PS);

};

bool MASK(long int registre,int numero_bit);

#endif
