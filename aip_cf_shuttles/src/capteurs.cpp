/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR
      Université Paul Sabatier Toulouse III 2016 ****/

#include "aip_cf_shuttles/capteurs.h"

Capteurs::Capteurs(ros::NodeHandle noeud)
{
  sub_capteurs_simu =
      noeud.subscribe("/commande/Simulation/Capteurs", 1,
                      &Capteurs::Callback_capteurs_simulation,this);
  for(int i=0;i<25;i++) PSx[i]=0;
  for(int i=0;i<13;i++) DxD[i]=0;
  for(int i=0;i<13;i++) DxG[i]=0;
  for(int i=0;i<11;i++) CPx[i]=0;
  for(int i=0;i<9;i++) CPIx[i]=0;
}

Capteurs::~Capteurs()
{
}

void Capteurs::Callback_capteurs_simulation
(const aip_cf_aiguillages::Msg_SensorState::ConstPtr& msg)
{
  for(int i=1;i<25;i++) PSx[i]=msg->PS[i];
  for(int i=1;i<13;i++) DxD[i]=msg->DD[i];
  for(int i=1;i<13;i++) DxG[i]=msg->DG[i];
  for(int i=1;i<11;i++) CPx[i]=msg->CP[i];
  for(int i=1;i<9;i++) CPIx[i]=msg->CPI[i];
}

bool Capteurs::get_PS(int num_PS)
{
  return PSx[num_PS];
}

bool Capteurs::get_DD(int num_DD)
{
  return DxD[num_DD];
}

bool Capteurs::get_DG(int num_DG)
{
  return DxG[num_DG];
}

bool Capteurs::get_CP(int num_CP)
{
  return CPx[num_CP];
}

bool Capteurs::get_CPI(int num_CPI)
{
  return CPIx[num_CPI];
}

bool MASK(long int registre,int numero_bit)
{
  return (bool)((registre & (long int)(pow(2,numero_bit)))/pow(2,numero_bit));
}
