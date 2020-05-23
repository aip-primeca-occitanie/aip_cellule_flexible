#include <ros/ros.h>
#include <unistd.h>
#include <queue>
#include "FileAttente.h"
#include <std_msgs/Int32.h>
#include "capteurs.h"
#include <std_msgs/Byte.h>
#include "aip_cf_shuttles/Msg_Erreur.h"


#include <iostream>
#include "aip_cf_shuttles/shuttle_id.h"

using namespace std;

int num_capteur;
vector<FileAttente*> liste_file;
int NbNavette=0;
int initPos=0;

bool shuttle_at_poste
(aip_cf_shuttles::shuttle_id::Request  &req,
 aip_cf_shuttles::shuttle_id::Response &res)
{
  switch (req.robot) {
    case 1:
      if (req.position==2){num_capteur=21;}
      if (req.position==3){num_capteur=22;}
      break;
    case 2:
      if (req.position==2){num_capteur=2;}
      if (req.position==3){num_capteur=3;}
      break;
    case 3:
      if (req.position==2){num_capteur=14;}
      if (req.position==3){num_capteur=15;}
      break;
    case 4:
      if (req.position==2){num_capteur=9;}
      if (req.position==3){num_capteur=10;}
      break;
  }

  if (!liste_file[num_capteur]->get_queue().empty())
  {
    res.IdShuttle=liste_file[num_capteur]->get_first_navette();
  }
  else
  {
    res.IdShuttle=66;
  }
  return true;
}


void initPosNavetteCallback(const std_msgs::Int32::ConstPtr& msg)
{
  NbNavette=msg->data;
  initPos=1;
}


void ShutdownCallback(const std_msgs::Byte::ConstPtr& msg)
{
  ros::shutdown();
}

int main(int argc, char **argv)
{
  cout << "debut Shuttle manager" << endl;
  ros::init(argc, argv, "Shuttle_manager");
  ros::NodeHandle noeud;

  ros::Subscriber subNbNavette =
      noeud.subscribe
      ("/commande_locale/nbNavettes", 100, &initPosNavetteCallback);

  ros::ServiceServer service =
      noeud.advertiseService
      ("get_id_shuttle_at_poste", shuttle_at_poste);
  ros::Subscriber sub_shutdown =
      noeud.subscribe
      ("/commande_locale/shutdown",10,&ShutdownCallback);
  ros::Publisher pub_erreur_log=
      noeud.advertise<aip_cf_shuttles::Msg_Erreur>
      ("/commande/Simulation/Erreur_log",10);

  Capteurs capteur(noeud);

  ros::Rate loop_rate(25); //fréquence de la boucle

  queue<int> queue_vide;
  queue<int> queue1;
  queue<int> queue2;
  queue<int> queue3;

  while (initPos==0)
  {
    ros::spinOnce();
    loop_rate.sleep();
  }
  for (int i=0; i<NbNavette; i++)
  {
    if (i==0)
    {
      queue3.push(0);
    }
    else if (i<4)
    {
      queue1.push(i);
    }
    else if (i<6)
    {
      queue2.push(i);
    }
  }

  //liste_file.push_back(FileAttente FileAttentePS(id_aiguillage,successeur_droite,successeur_gauche,queue));
  liste_file.push_back(0);
  liste_file.push_back(new FileAttente(1,1+24,2+24,queue_vide));//PS1
  liste_file.push_back(new FileAttente(0,3,3,queue_vide));//PS2
  liste_file.push_back(new FileAttente(0,5,5,queue3));
  liste_file.push_back(new FileAttente(2,0,6,queue_vide));
  liste_file.push_back(new FileAttente(2,6,0,queue_vide));
  liste_file.push_back(new FileAttente(3,24+3,20,queue_vide)); // Deux aiguillages ??
  liste_file.push_back(new FileAttente(4,8,0,queue2));
  liste_file.push_back(new FileAttente(5,4+24,5+24,queue_vide));
  liste_file.push_back(new FileAttente(0,10,10,queue_vide));
  liste_file.push_back(new FileAttente(0,12,12,queue_vide));
  liste_file.push_back(new FileAttente(6,0,13,queue_vide));
  liste_file.push_back(new FileAttente(6,13,0,queue_vide));
  liste_file.push_back(new FileAttente(7,6+24,7+24,queue_vide));
  liste_file.push_back(new FileAttente(0,15,15,queue_vide));
  liste_file.push_back(new FileAttente(0,17,17,queue_vide));
  liste_file.push_back(new FileAttente(8,0,18,queue_vide));
  liste_file.push_back(new FileAttente(8,18,0,queue_vide));
  liste_file.push_back(new FileAttente(9,8+24,8,queue_vide));
  liste_file.push_back(new FileAttente(10,20,0,queue1));
  liste_file.push_back(new FileAttente(11,9+24,10+24,queue_vide));
  liste_file.push_back(new FileAttente(0,22,22,queue_vide));
  liste_file.push_back(new FileAttente(0,24,24,queue_vide));
  liste_file.push_back(new FileAttente(12,0,1,queue_vide));
  liste_file.push_back(new FileAttente(12,1,0,queue_vide));

  liste_file.push_back(new FileAttente(0,2,2,queue_vide)); //CP1
  liste_file.push_back(new FileAttente(0,4,4,queue_vide)); //CP2
  liste_file.push_back(new FileAttente(0,7,7,queue_vide));
  liste_file.push_back(new FileAttente(0,9,9,queue_vide));
  liste_file.push_back(new FileAttente(0,11,11,queue_vide));
  liste_file.push_back(new FileAttente(0,14,14,queue_vide));
  liste_file.push_back(new FileAttente(0,16,16,queue_vide));
  liste_file.push_back(new FileAttente(0,19,19,queue_vide));
  liste_file.push_back(new FileAttente(0,21,21,queue_vide));
  liste_file.push_back(new FileAttente(0,23,23,queue_vide));

  vector<int> mem_capteur;
  vector<int> etat_capteur;
  //vector<queue<int>> debug_display;

  int file_attente_suivante;
  int id_aiguillage;

  for (int i=1;i<36;i++)
  {
    etat_capteur.push_back(0);
  }

  while (ros::ok())
  {
    mem_capteur=etat_capteur;
    etat_capteur.clear();
    etat_capteur.push_back(0);
    //debug_display.push_back(queue_vide);

    for (int i=1;i<25;i++)
    {
      etat_capteur.push_back(capteur.get_PS(i));
    }
    for (int i=1;i<11;i++)
    {
      etat_capteur.push_back(capteur.get_CP(i));
    }

    for (int i=1;i<35;i++)
    {
      if(etat_capteur[i]==0 && mem_capteur[i]==1)
      {
        id_aiguillage=liste_file[i]->get_id_aiguillage();
        file_attente_suivante=liste_file[i]->maj(capteur.get_DD(id_aiguillage),capteur.get_DG(id_aiguillage));
        if (file_attente_suivante>0)
        {
          liste_file[file_attente_suivante]->add_navette_in_queue(liste_file[i]->get_first_navette());
          liste_file[i]->delete_navette_in_queue();
        }
        if (file_attente_suivante==-1)
        {
          liste_file[i]->delete_navette_in_queue();
          aip_cf_shuttles::Msg_Erreur msgErreur;
          msgErreur.code=4;
          msgErreur.n_poste=i;
          pub_erreur_log.publish(msgErreur);
        }
        file_attente_suivante=-2;
      }

      //debug_display.push_back(liste_file[i]->get_queue());
    }

    /*for (int i=1;i<35;i++)
      {
      cout << "La file " << i <<" contient : ";
      while (!debug_display[i].empty())
      {
      cout << ' ' << debug_display[i].front();
      debug_display[i].pop();
      }
      cout << endl;
      }
      cout<<endl;
      debug_display.clear();*/

    mem_capteur.clear();

    ros::spinOnce(); //permet aux fonction callback de ros dans les objets d'êtres appelées
    loop_rate.sleep(); //permet de synchroniser la boucle while. Il attend le temps qu'il reste pour faire le 25Hz (ou la fréquence indiquée dans le loop_rate)
  }

  return 0;
}
