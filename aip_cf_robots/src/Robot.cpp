/**** Projet long N7 2020 ****/
#include "Robot.h"

using namespace std;

//Constructeur
Robot::Robot(int num_du_robot)
{
  //Valeur de pi
  pi=3.14159265359;
  num_robot=num_du_robot;
  retour.num_robot=num_robot;
  ROS_INFO("le numero du robot est %d ", num_robot);
  for (int i=0;i<7;i++)
  {
    //ATTENTION : Ne pas changer la valeur d'initialisation
    //l'ensemble des setModes doit être à 1 pour que les joints des robots soient commandés
    mymodes[i]=1;
  }

  for(int i=0; i<NB_CUBE; i++)
    couleur_transportee[i]=0;

  repSim_getObjectHandle=false;
  repSim_setJointState=false;
  repSim_getJointState=false;
  repSim_getTime=false;
  repSim_changeColor=false;
  repSim_changeShuttleColor=false;
  repSim_getColor=false;
  repSim_getColorUpdate=false;

  msgSim_setJointState.layout.dim.push_back(std_msgs::MultiArrayDimension());
  msgSim_setJointState.layout.dim[0].label="handles";
  msgSim_setJointState.layout.dim[0].size=7; // On utilise que cette valeur
  msgSim_setJointState.layout.dim.push_back(std_msgs::MultiArrayDimension());
  msgSim_setJointState.layout.dim[0].label="values";
  msgSim_setJointState.layout.dim[0].size=7;

  loop_rate = new ros::Rate(25);
  loop_ok = new ros::Rate(2);
}

//Destructeur
Robot::~Robot()
{
  delete loop_rate;
  delete loop_ok;
}

/** Pour atteindre une position prédéfinie **/
//Fonction permettant d'envoyer le robot dans une position prédéfinie
void Robot::EnvoyerRobot(int numposition)
{
  robotPosition.data=numposition;
  //Choix de la position en fonction du numéro reçu
  switch(numposition)
  {
    case 1:
      Rpos[0]=128*pi/180;       // attributs correspondant au position (7 axes du robot)
      Rpos[1]=90*pi/180;
      Rpos[2]=90*pi/180;
      Rpos[3]=80*pi/180;
      Rpos[4]=90*pi/180;
      Rpos[5]=-90*pi/180;
      Rpos[6]=-40*pi/180;
      break;

    case 2:
      Rpos[0]=60*pi/180;
      Rpos[1]=90*pi/180;
      Rpos[2]=90*pi/180;
      Rpos[3]=80*pi/180;
      Rpos[4]=90*pi/180;
      Rpos[5]=-90*pi/180;
      Rpos[6]=-20*pi/180;
      break;

    case 3:
      Rpos[0]=15*pi/180;
      Rpos[1]=90*pi/180;
      Rpos[2]=90*pi/180;
      Rpos[3]=80*pi/180;
      Rpos[4]=90*pi/180;
      Rpos[5]=-90*pi/180;
      Rpos[6]=-65*pi/180;
      break;

    case 4:
      Rpos[0]=-53*pi/180;
      Rpos[1]=90*pi/180;
      Rpos[2]=90*pi/180;
      Rpos[3]=80*pi/180;
      Rpos[4]=90*pi/180;
      Rpos[5]=-90*pi/180;
      Rpos[6]=-40*pi/180;
      break;
  }


  // Utilisation du topic SetJointState pour envoyer le robot dans la position souhaitée
  msgSim_setJointState.data.clear();
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back((float)Rints[i]);
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back(Rpos[i]);

  pubSim_setJointState.publish(msgSim_setJointState);

  //Attente de la réponse
  while(!repSim_setJointState&&ros::ok())
  {
    ros::spinOnce();
    loop_rate->sleep();
  }
  repSim_setJointState=false;


  //Utilisation du topic GetJointState pour savoir si le mouvement du robot est terminé :
  //on considère que la position est atteinte si la position est à 0.001 de sa consigne
  float Position;
  for(int i=0;i<7;i++)
  {
    Position=0;

    //Appel du topic pour connaître la position du robot
    msgSim_getJointState.data = Rints[i];
    pubSim_getJointState.publish(msgSim_getJointState);
    while(!repSim_getJointState&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getJointState = false;
    Position = valueSim_getJointState;

    //Attente jusqu'à ce que la position soit atteinte
    while(abs(Position-Rpos[i])>=0.001&&ros::ok())
    {
      pubSim_getJointState.publish(msgSim_getJointState);
      while(!repSim_getJointState&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getJointState=false;
      Position = valueSim_getJointState;

      ros::spinOnce();
      loop_ok->sleep();
    }
  }

  ROS_INFO("Position atteinte robot %d", num_robot);

  //Retour vers la commande
  retour.data = 2;
  pub_retourCommande.publish(retour);

  pub_robotPosition.publish(robotPosition);
}

/** Pour atteindre une position définie manuellement **/
//Fonction permettant d'envoyer le robot dans une position définie manuellement
void Robot::EnvoyerJoints(int joint1, int joint2, int joint3, int joint4, int joint5, int joint6, int joint7)
{
  //Récupération des angles choisis par l'utilisateur
  Rpos[0]=(joint1)*pi/180;
  Rpos[1]=(joint2)*pi/180;
  Rpos[2]=(joint3)*pi/180;
  Rpos[3]=(joint4)*pi/180;
  Rpos[4]=(joint5)*pi/180;
  Rpos[5]=(joint6)*pi/180;
  Rpos[6]=(joint7)*pi/180;


  // Utilisation du topic SetJointState pour envoyer le robot dans la position souhaitée
  msgSim_setJointState.data.clear();
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back((float)Rints[i]);
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back(Rpos[i]);

  pubSim_setJointState.publish(msgSim_setJointState);

  //Attente de la réponse
  while(!repSim_setJointState&&ros::ok())
  {
    ros::spinOnce();
    loop_rate->sleep();
  }
  repSim_setJointState=false;


  //Utilisation du topic GetJointState pour savoir si le mouvement du robot est terminé
  //On considère que la position est atteinte si la position est à 0.001 de sa consigne
  float Position;
  for(int i=0;i<7;i++)
  {
    Position=0;

    //Appel du topic pour connaître la position du robot
    msgSim_getJointState.data = Rints[i];
    pubSim_getJointState.publish(msgSim_getJointState);
    while(!repSim_getJointState&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getJointState = false;
    Position = valueSim_getJointState;

    //Attente jusqu'à ce que la position soit atteinte
    while(abs(Position-Rpos[i])>=0.001&&ros::ok())
    {
      pubSim_getJointState.publish(msgSim_getJointState);
      while(!repSim_getJointState&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getJointState=false;
      Position = valueSim_getJointState;


      ros::spinOnce();
      loop_ok->sleep();
    }
  }

  ROS_INFO("Position atteinte %d", num_robot);

  //Retour vers la commande
  retour.data = 2;
  pub_retourCommande.publish(retour);
}

/** Pour descendre ou monter le bras **/
//Fonction permettant de mettre le bras en position basse
void Robot::DescendreBras()
{
  //Récupération et modification de la position actuelle
  Rpos[0]=Rpos[0]+2*pi/180;
  Rpos[1]=Rpos[1]+10*pi/180;
  Rpos[2]=Rpos[2];
  Rpos[3]=Rpos[3];
  Rpos[4]=Rpos[4]+10*pi/180;
  Rpos[5]=Rpos[5]+3*pi/180;
  Rpos[6]=Rpos[6]+6*pi/180;


  // Utilisation du topic SetJointState pour envoyer le robot dans la position souhaitée
  msgSim_setJointState.data.clear();
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back((float)Rints[i]);
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back(Rpos[i]);

  pubSim_setJointState.publish(msgSim_setJointState);

  //Attente de la réponse
  while(!repSim_setJointState&&ros::ok())
  {
    ros::spinOnce();
    loop_rate->sleep();
  }
  repSim_setJointState=false;

  //Utilisation du topic GetJointState pour savoir si le mouvement du robot est terminé
  //On considère que la position est atteinte si la position est à 0.001 de sa consigne
  float Position;
  for(int i=0;i<7;i++)
  {
    Position=0;

    //Appel du topic pour connaître la position du robot
    msgSim_getJointState.data = Rints[i];
    pubSim_getJointState.publish(msgSim_getJointState);
    while(!repSim_getJointState&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getJointState = false;
    Position = valueSim_getJointState;

    //Attente jusqu'à ce que la position soit atteinte
    while(abs(Position-Rpos[i])>=0.001&&ros::ok())
    {
      pubSim_getJointState.publish(msgSim_getJointState);
      while(!repSim_getJointState&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getJointState=false;
      Position = valueSim_getJointState;

      ros::spinOnce();
      loop_ok->sleep();
    }
  }


  ROS_INFO("Bras descendu pour le robot %d", num_robot);

  //Retour vers la commande
  retour.data = 4;
  pub_retourCommande.publish(retour);

  //Retour de l'état actuel du bras
  robotBras.data = 0;
  pub_robotBras.publish(robotBras);
}

//Fonction permettant de mettre le bras en position haute
void Robot::MonterBras()
{
  //Récupération et modification de la position actuelle
  Rpos[0]=Rpos[0]-2*pi/180;
  Rpos[1]=Rpos[1]-10*pi/180;
  Rpos[2]=Rpos[2];
  Rpos[3]=Rpos[3];
  Rpos[4]=Rpos[4]-10*pi/180;
  Rpos[5]=Rpos[5]-3*pi/180;
  Rpos[6]=Rpos[6]-6*pi/180;


  // Utilisation du topic SetJointState pour envoyer le robot dans la position souhaitée
  msgSim_setJointState.data.clear();
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back((float)Rints[i]);
  for(int i=0; i<7; i++)
    msgSim_setJointState.data.push_back(Rpos[i]);

  pubSim_setJointState.publish(msgSim_setJointState);

  //Attente de la réponse
  while(!repSim_setJointState&&ros::ok())
  {
    ros::spinOnce();
    loop_rate->sleep();
  }
  repSim_setJointState=false;

  //Utilisation du topic GetJointState pour savoir si le mouvement du robot est terminé
  //On considère que la position est atteinte si la position est à 0.001 de sa consigne
  float Position;
  for(int i=0;i<7;i++)
  {
    Position=0;

    //Appel du topic pour connaître la position du robot
    msgSim_getJointState.data = Rints[i];
    pubSim_getJointState.publish(msgSim_getJointState);
    while(!repSim_getJointState&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getJointState = false;
    Position = valueSim_getJointState;

    //Attente jusqu'à ce que la position soit atteinte
    while(abs(Position-Rpos[i])>=0.001&&ros::ok())
    {
      pubSim_getJointState.publish(msgSim_getJointState);
      while(!repSim_getJointState&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getJointState=false;
      Position = valueSim_getJointState;


      ros::spinOnce();
      loop_ok->sleep();
    }
  }


  ROS_INFO("Bras monte pour le robot %d",num_robot);

  //Retour vers la commande
  retour.data = 5;
  pub_retourCommande.publish(retour);

  //Retour de l'état actuel du bras
  robotBras.data = 1;
  pub_robotBras.publish(robotBras);
}

/** Pour fermer ou ouvrir la pince **/
//Fonction permettant de fermer la pince du robot en envoyant une commande sur le topic correspondant
void Robot::FermerPince()
{
  //Message de commande de fermeture de la pince (1 pour fermer, 0 pour ouvrir)
  std_msgs::Int32 cmd;
  cmd.data = 1;

  pub_pince.publish(cmd);

  //Si la pince n'est pas déjà fermée, un retour est envoyé à la commande
  if(retour.data != 6)
  {
    //Attente pour que la pince se ferme
    float t0, time;

    pubSim_getTime.publish(msgSim_getTime);
    while(!repSim_getTime&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getTime=false;
    t0 = valueSim_getTime;

    time = t0;
    while(time - t0 < 0.5 &&ros::ok())
    {
      pubSim_getTime.publish(msgSim_getTime);
      while(!repSim_getTime&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getTime=false;
      time = valueSim_getTime;

      ros::spinOnce();
      loop_ok->sleep();
    }

    //Retour vers la commande
    retour.data = 6;
    pub_retourCommande.publish(retour);
  }

  //Retour de l'état de la pince
  robotPince.data=1;
  pub_robotPince.publish(robotPince);
}

//Fonction permettant d'ouvrir la pince du robot en envoyant une commande sur le topic correspondant
void Robot::OuvrirPince()
{
  //Message de commande d'ouverture de la pince (1 pour fermer, 0 pour ouvrir)
  std_msgs::Int32 cmd;
  cmd.data = 0;

  pub_pince.publish(cmd);

  //Si la pince n'est pas déjà ouverte, un retour est envoyé à la commande
  if(retour.data != 7)
  {
    //Attente pour que la pince s'ouvre
    float t0, time;

    pubSim_getTime.publish(msgSim_getTime);
    while(!repSim_getTime&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getTime=false;
    t0 = valueSim_getTime;

    time = t0;
    while(time - t0 < 0.5&&ros::ok())
    {
      pubSim_getTime.publish(msgSim_getTime);
      while(!repSim_getTime&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getTime=false;
      time = valueSim_getTime;

      ros::spinOnce();
      loop_ok->sleep();
    }

    //Retour vers la commande
    retour.data = 7;
    pub_retourCommande.publish(retour);
  }

  //Retour de l'état de la pince
  robotPince.data=0;
  pub_robotPince.publish(robotPince);
}

/*** Fonctions permettant de controler le robot avec des ordres du noeud commande ***/
/** Envoyer le robot automatiquement **/
//Fonction Callback permettant d'envoyer le robot dans une position prédéfinie à la réception du message de Commande
void Robot::SendPositionCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    //Récupération des données du message : numéro de la position prédéfinie
    int pos;
    pos = msg->data;

    //Envoi du robot dans la position choisie
    EnvoyerRobot(pos);
  }
}

/** Envoyer le robot manuellement **/
//Fonction Callback permettant d'envoyer le robot dans une position choisie par l'utilisateur à la réception du message de Commande
void Robot::SendJointsCallback(const aip_cf_commande_locale::RobotJoints::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    EnvoyerJoints(msg->joint1, msg->joint2, msg->joint3, msg->joint4, msg->joint5, msg->joint6, msg->joint7);
  }
}

/** Fermer la pince **/
//Fonction Callback permettant de fermer la pince du robot à la réception du message de Commande
void Robot::FermerPinceCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    FermerPince();
  }
}
/** Ouvrir la pince **/
//Fonction Callback permettant d'ouvrir la pince du robot à la réception du message de Commande
void Robot::OuvrirPinceCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    OuvrirPince();
  }
}

/** Descendre le bras **/
//Fonction Callback permettant de mettre le bras en position basse à la réception du message de Commande
void Robot::DescendreBrasCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    DescendreBras();
  }
}

/** Monter le bras **/
//Fonction Callback permettant de mettre le bras en position haute à la réception du message de Commande
void Robot::MonterBrasCallback(const aip_cf_robots::Msg_numrobot::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    MonterBras();
  }
}

/** Contrôler le robot entièrement **/
//Fonction Callback permettant de contrôler l'ensemble des mouvements du robot à la réception du message de Commande
void Robot::ControlerRobotCallback(const aip_cf_robots::MoveRobot::ConstPtr& msg)
{
  if(num_robot==msg->num_robot)
  {
    //Envoi du robot dans la position souhaitée
    EnvoyerRobot(msg->position);

    //Envoi du bras dans l'état souhaité
    switch(msg->bras)
    {
      case -1:
        DescendreBras();
        break;

      case 1:
        MonterBras();
        break;
    }

    //Envoi de la pince dans l'état souhaité
    switch(msg->pince)
    {
      case -1:
        OuvrirPince();
        break;

      case 1:
        FermerPince();
        break;
    }
  }
}

int Robot::computeTableId(int position)
{
  int id=-1;

  switch(num_robot)
  {
    case 1:
      if(position==1)
        id=1;
      else if(position==4)
        id=0;
      break;
    case 2:
      if(position==1)
        id=3;
      else if(position==4)
        id=4;
      break;
    case 3:
      if(position==1)
        id=7;
      else if(position==4)
        id=6;
      break;
    case 4:
      if(position==1)
        id=9;
      else if(position==4)
        id=10;
      break;
  }

  return id;
}

void Robot::Colorer(int position, int type)//attention c'est forcement quand on transporte !!
{
  // type==0 <=> prise /  =1 <=> pose
  int idNavette=-1;
  if(position==2 || position==3) // Si navette
  {
    srv.request.robot=num_robot;
    srv.request.position=position;
    client.call(srv);
    idNavette = srv.response.IdShuttle;
    ROS_INFO("Navette %d", idNavette);
  }

  if(idNavette==66) // 66=Erreur
  {
    ROS_ERROR("ERREUR : Pas de navette a la position demandee");	
    for(int i=0; i<NB_CUBE; i++)
      couleur_transportee[i]=0;
    transport(false);
  }
  else
  {
    // regarde la couleur de ce qu'on veut prendre
    bool couleur_vide=true;
    int couleur[NB_CUBE];
    char c[2];
    if(idNavette==0)
      c[0]=(char)(idNavette+90);
    else
      c[0]=(char)(idNavette+64);
    c[1]='\0';
    string signal;
    if(position==2 || position==3) // Si navette
      signal="Shuttle"+string(c);
    else if(position==1 || position==4) // Si poste
    {
      if(position==1)
      {
        signal=poste_pos_1.get_nom();
        if(poste_pos_1.isTaskEnCours())
        {
          // Log
          ROS_ERROR("Manipulation d'une piece en cours de traitement ! [poste=%d]", poste_pos_1.get_numero());
          aip_cf_commande_locale::Msg_Erreur msgErreur;
          msgErreur.code=3;
          msgErreur.n_poste=poste_pos_1.get_numero();
          pub_erreur_log.publish(msgErreur);

          // Interrompt tache
          poste_pos_1.stopTask();
          retour.data=8;
          pub_retourCommande.publish(retour);
        }
      }
      else
      {
        signal=poste_pos_4.get_nom();
        if(poste_pos_4.isTaskEnCours())
        {
          // Log
          ROS_ERROR("Manipulation d'une piece en cours de traitement ! [poste=%d]", poste_pos_4.get_numero());
          aip_cf_commande_locale::Msg_Erreur msgErreur;
          msgErreur.code=3;
          msgErreur.n_poste=poste_pos_4.get_numero();
          pub_erreur_log.publish(msgErreur);

          // Interrompt tache
          poste_pos_4.stopTask();
          retour.data=9;
          pub_retourCommande.publish(retour);
        }
      }
    }

    string fin;

    for(int i=0; i<NB_CUBE; i++)
    {
      fin.clear();
      fin.append(signal);
      fin.append("#");
      fin.append(to_string(i));
      fin.append("_color");
      msgSim_getColor.data=fin;

      pubSim_getColor.publish(msgSim_getColor);
      while(!repSim_getColor&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getColor=false;
      couleur[i]=valueSim_getColor;
    }
    for(int i=0; i<NB_CUBE; i++)
    {
      if(couleur[i]!=0)
        couleur_vide=false;
    }

    // colore le poste ou navette en pos 1 avec couleur en mémoire
    if(position==2 || position==3) // Si navette
    {
      msgSim_changeShuttleColor.data.clear();
      msgSim_changeShuttleColor.data.push_back(idNavette);
      for(int i=0; i<NB_CUBE; i++)
        msgSim_changeShuttleColor.data.push_back(couleur_transportee[i]);
      pubSim_changeShuttleColor.publish(msgSim_changeShuttleColor);
      while(!repSim_changeShuttleColor&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_changeShuttleColor=false;
    }
    else if(position==1 || position==4)
    {
      msgSim_changeColor.data.clear();
      msgSim_changeColor.data.push_back(computeTableId(position));
      for(int i=0; i<NB_CUBE; i++)
        msgSim_changeColor.data.push_back(couleur_transportee[i]);
      pubSim_changeColor.publish(msgSim_changeColor);
      while(!repSim_changeColor&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_changeColor=false;
    }

    // on met a jour la couleur en mémoire (qu'on transporte) seulement si prise
    for(int i=0; i<NB_CUBE; i++)
    {
      if(type==0)
        couleur_transportee[i]=couleur[i];
      else
        couleur_transportee[i]=0;
      cout << "couleur_trasportee[" << i << "]=" << couleur_transportee[i] << endl;
    }

    // Mise jour modele pince (si tiens quelque chose, non vide)
    if(type==0 && !couleur_vide)
      transport(true);
    else
      transport(false);

    // Detecte si on a écrasé un produit
    if(type==1 && !couleur_vide)
    {
      ROS_ERROR("ON A ECRASE UN PRODUIT !!!");
      msg_erreur.code=66; //66=code ecrasement produit
      msg_erreur.n_poste=num_robot;
      pub_erreur_log.publish(msg_erreur);
    }
  }
}

int Robot::colorerPosteDebutTask(int positionPoste)
{
  string signal;
  string fin;
  int couleur[NB_CUBE];
  for(int i=0; i<NB_CUBE; couleur[i++]=0){}
  int couleur_last(0);
  int retour=-1;
  int n_poste;
  int couleur_a_ajouter;

  if(positionPoste==1)
  {
    signal=poste_pos_1.get_nom();
    n_poste=poste_pos_1.get_numero();
    couleur_a_ajouter=poste_pos_1.get_color()-1; // -1 pour opacité 50%
  }
  if(positionPoste==4)
  {
    signal=poste_pos_4.get_nom();
    n_poste=poste_pos_4.get_numero();
    couleur_a_ajouter=poste_pos_4.get_color()-1;
  }

  // On cherche la 1ere case vide
  int i=0;
  do
  {
    fin.clear();
    fin.append(signal);
    fin.append("#");
    fin.append(to_string(i));
    fin.append("_color");
    msgSim_getColor.data=fin;

    pubSim_getColor.publish(msgSim_getColor);
    while(!repSim_getColor&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getColor=false;
    couleur[i]=valueSim_getColor;
    couleur_last=couleur[i];

    i++;

  }while(i<NB_CUBE && couleur_last!=0&&ros::ok());

  if(i==1)
  {
    ROS_ERROR("TACHE SUR AUCUN PRODUIT !!!");
    msg_erreur.code=1;
    msg_erreur.n_poste=n_poste;
    pub_erreur_log.publish(msg_erreur);
  }

  else if(i==NB_CUBE && couleur_last!=0)
  {
    ROS_ERROR("PRODUIT PLEIN !!!");
    msg_erreur.code=2;
    msg_erreur.n_poste=n_poste;
    pub_erreur_log.publish(msg_erreur);
  }
  else
  {
    // mettre couleur sur signal/case i-1
    string idStr= signal.substr(6);
    int idPoste = atoi(idStr.c_str());
    msgSim_changeColor.data.clear();
    msgSim_changeColor.data.push_back(idPoste);

    couleur[i-1]=couleur_a_ajouter;
    cout << "couleur_a_ajouter=" << couleur_a_ajouter << endl;

    for(int j=0; j<NB_CUBE; j++)
      msgSim_changeColor.data.push_back(couleur[j]);
    pubSim_changeColor.publish(msgSim_changeColor);
    while(!repSim_changeColor&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_changeColor=false;

    retour = i-1;
  }

  return retour;
}

int Robot::colorerPosteFinTask(int positionPoste, int duree)
{
  string signal;
  string fin;
  int couleur[NB_CUBE];
  for(int i=0; i<NB_CUBE; couleur[i++]=0){}
  int couleur_last(0);
  int retour=-1;
  int n_poste;
  int couleur_a_ajouter;

  if(positionPoste==1)
  {
    signal=poste_pos_1.get_nom();
    n_poste=poste_pos_1.get_numero();
    couleur_a_ajouter=poste_pos_1.get_color();
  }
  if(positionPoste==4)
  {
    signal=poste_pos_4.get_nom();
    n_poste=poste_pos_4.get_numero();
    couleur_a_ajouter=poste_pos_4.get_color();
  }

  // On cherche 1ere case vide
  int i=0;
  do
  {
    fin.clear();
    fin.append(signal);
    fin.append("#");
    fin.append(to_string(i));
    fin.append("_color");
    msgSim_getColor.data=fin;

    pubSim_getColor.publish(msgSim_getColor);
    while(!repSim_getColor&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getColor=false;
    couleur[i]=valueSim_getColor;
    couleur_last=couleur[i];

    i++;

  }while(i<NB_CUBE && couleur_last!=0&&ros::ok());

  // mettre couleur sur signal i-1
  string idStr= signal.substr(6);
  int idPoste = atoi(idStr.c_str());
  msgSim_changeColor.data.clear();
  msgSim_changeColor.data.push_back(idPoste);

  if(i==NB_CUBE && couleur_last!=0)
  {
    couleur[i-1]=couleur_a_ajouter;
    retour = i-1;
  }
  else
  {
    couleur[i-2]=couleur_a_ajouter;
    retour = i-2;
  }
  cout << "couleur_a_ajouter=" << couleur_a_ajouter << endl;

  for(int j=0; j<NB_CUBE; j++)
    msgSim_changeColor.data.push_back(couleur[j]);
  pubSim_changeColor.publish(msgSim_changeColor);
  while(!repSim_changeColor&&ros::ok())
  {
    ros::spinOnce();
    loop_rate->sleep();
  }
  repSim_changeColor=false;

  // pour le log
  ROS_INFO("Task Po:%d, Pr:%d, Du%d",n_poste,couleur[0],duree);
  msg_tache_finie.num_poste=n_poste;
  msg_tache_finie.num_produit=couleur[0];
  msg_tache_finie.duree=duree;
  pub_tache_finie.publish(msg_tache_finie);

  return retour;
}

void Robot::faireTacheCallback(const aip_cf_robots::FaireTacheMsg::ConstPtr& msg)
{
  if((msg->num_robot==num_robot)
     && (msg->position==1||msg->position==4)) // pas sur une navette
  {
    ROS_INFO("Debut tache pos=%d", msg->position);
    pubSim_getTime.publish(msgSim_getTime);
    while(!repSim_getTime && ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getTime=false;
    float time=valueSim_getTime;

    int retourDebTask = colorerPosteDebutTask(msg->position);
    if(msg->position==1)
    {
      if(poste_pos_1.isTaskEnCours())
        ROS_ERROR("ERREUR : Nouvelle tache pendant une tache en cours !");
      //  commence la tache seulement si retour ok
      if(retourDebTask!=-1)
        poste_pos_1.debutTask(time,msg->duree);
      else
      {
        retour.data=8;
        pub_retourCommande.publish(retour);
      }
    }
    else
    {
      if(poste_pos_4.isTaskEnCours())
        ROS_ERROR("ERREUR : Nouvelle tache pendant une tache en cours !");
      //  commence la tache seulement si retour ok
      if(retourDebTask!=-1)
        poste_pos_4.debutTask(time,msg->duree);
      else
      {
        retour.data=9;
        pub_retourCommande.publish(retour);
      }
    }
  }
}

void Robot::update()
{
  // Si une tache est en cours
  if(poste_pos_1.isTaskEnCours() || poste_pos_4.isTaskEnCours())
  {
    // On demande le temps à VREP
    pubSim_getTimeUpdate.publish(msgSim_getTimeUpdate);
    while(!repSim_getTimeUpdate && ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_getTimeUpdate=false;
    float time=valueSim_getTimeUpdate;
    cout << endl;

    // Si la tache poste pos 1 en cours, on update
    if(poste_pos_1.isTaskEnCours())
    {
      // On update et verifie si la tache poste pos 1 est finie
      if(poste_pos_1.updateTask(time)) // si tache finie
      {
        int indice=colorerPosteFinTask(1,poste_pos_1.get_duree());
        if(indice==-1)
          ROS_ERROR("ColorerPosteTask Probleme !!");
        string signal=poste_pos_1.get_nom();
        string fin;
        fin.append(signal);
        fin.append("#");
        fin.append(to_string(indice));
        fin.append("_color");
        msgSim_getColor.data=fin;
        int couleur;

        do
        {
          pubSim_getColor.publish(msgSim_getColor);
          while(!repSim_getColor&&ros::ok())
          {
            ros::spinOnce();
            loop_rate->sleep();
          }
          repSim_getColor=false;
          couleur=valueSim_getColor;

          loop_rate->sleep();
        }while(couleur!=poste_pos_1.get_color()&&ros::ok());

        retour.data=8;
        pub_retourCommande.publish(retour);
      }
    }

    // Si la tache poste pos 4 est en cours, on update
    if(poste_pos_4.isTaskEnCours())
    {
      // On update et verifie si la tache poste pos 4 est finie
      if(poste_pos_4.updateTask(time)) // si tache finie
      {
        int indice=colorerPosteFinTask(4,poste_pos_4.get_duree());
        if(indice==-1)
          ROS_ERROR("ColorerPosteTask Probleme !!");
        string signal=poste_pos_4.get_nom();
        string fin;
        fin.append(signal);
        fin.append("#");
        fin.append(to_string(indice));
        fin.append("_color");
        msgSim_getColor.data=fin;
        int couleur;

        do
        {
          pubSim_getColor.publish(msgSim_getColor);
          while(!repSim_getColor&&ros::ok())
          {
            ros::spinOnce();
            loop_rate->sleep();
          }
          repSim_getColor=false;
          couleur=valueSim_getColor;

          loop_rate->sleep();
        }while(couleur!=poste_pos_4.get_color()&&ros::ok());
        retour.data=9;
        pub_retourCommande.publish(retour);
      }
    }
  }
}

void Robot::transport(bool valeur)
{
  std_msgs::Bool msg;
  msg.data=valeur;
  pub_robot_transport.publish(msg);
}

// On definit qui evacue et comment
void Robot::Evacuer(const std_msgs::Byte::ConstPtr& msg)
{
  if(num_robot==2)
  {
    cout << "Evacuer" << endl;
    int position=1;  // on evacue sur la position 1 du robot 2 <=> poste 3

    int couleur[NB_CUBE];
    string signal=poste_pos_1.get_nom();
    msg_log_couleur.data.clear();
    string fin;
    for(int i=0; i<NB_CUBE; i++)
    {
      fin.clear();
      fin.append(signal);
      fin.append("#");
      fin.append(to_string(i));
      fin.append("_color");
      msgSim_getColor.data=fin;

      pubSim_getColor.publish(msgSim_getColor);

      while(!repSim_getColor&&ros::ok())
      {
        ros::spinOnce();
        loop_rate->sleep();
      }
      repSim_getColor=false;
      couleur[i]=valueSim_getColor;
      msg_log_couleur.data.push_back(couleur[i]);
      cout << "couleur[" << i << "]=" << couleur[i] << endl;
    }
		
    //pour le log
    pub_produitEvac.publish(msg_log_couleur);

    // On fait disparaitre
    msgSim_changeColor.data.clear();
    msgSim_changeColor.data.push_back(computeTableId(position));
    for(int i=0; i<NB_CUBE; i++)
      msgSim_changeColor.data.push_back(0);
    pubSim_changeColor.publish(msgSim_changeColor);
    while(!repSim_changeColor&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep();
    }
    repSim_changeColor=false;
  }
}

void Robot::stopTacheCallback(const std_msgs::Int32::ConstPtr& msg)
{
  if(msg->data==1) //pos
  {
    poste_pos_1.stopTask();
    retour.data=8;
    pub_retourCommande.publish(retour);
  }
  else if(msg->data==4)
  {
    poste_pos_4.stopTask();
    retour.data=9;
    pub_retourCommande.publish(retour);
  }
}

void Robot::DeplacerPieceCallback(const aip_cf_commande_locale::DeplacerPieceMsg::ConstPtr& msg)
{
  if (num_robot==msg->num_robot)
  {
    EnvoyerRobot(msg->positionA);
    DescendreBras();

    // Prise de pièce
    //le robot a rien en mémoire, il décolore
    Colorer(msg->positionA,0);
    FermerPince();

    MonterBras();
    EnvoyerRobot(msg->positionB);
    DescendreBras();

    // Pose de pièce
    //le robot a la couleur du produit en memoire, il colore
    Colorer(msg->positionB,1);
    OuvrirPince();

    MonterBras();
    retour.data = 10;
    pub_retourCommande.publish(retour);
  }
}

/*** Initialisation ***/
//Initialisation des services, des publishers et des subscribers + Récupération des handles des robots
void Robot::init(ros::NodeHandle noeud)
{
  string nom;
  int numero_poste;
  switch(num_robot){

    case 1:
      nom="Table#1";
      numero_poste=2;
      poste_pos_1.init(nom,numero_poste);
      nom="Table#0";
      numero_poste=1;
      poste_pos_4.init(nom,numero_poste);
      break;

    case 2:
      nom="Table#3";
      numero_poste=3;
      poste_pos_1.init(nom,numero_poste);
      nom="Table#4";
      numero_poste=4;
      poste_pos_4.init(nom,numero_poste);
      break;

    case 3:
      nom="Table#7";
      numero_poste=6;
      poste_pos_1.init(nom,numero_poste);
      nom="Table#6";
      numero_poste=5;
      poste_pos_4.init(nom,numero_poste);
      break;

    case 4:
      nom="Table#9";
      numero_poste=7;
      poste_pos_1.init(nom,numero_poste);
      nom="Table#10";
      numero_poste=8;
      poste_pos_4.init(nom,numero_poste);
      break;

    default:
      ROS_INFO("CHOIX ROBOT INCORRECT");
      break;
  }

  // Topic pour V-Rep
  pubSim_getObjectHandle = noeud.advertise<std_msgs::String>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/GetObjectHandle",100);
  subSim_getObjectHandle = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/GetObjectHandle",100,&Robot::simGetObjectHandleCallback,this);

  pubSim_setJointState = noeud.advertise<std_msgs::Float32MultiArray>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/SetJointState",100);
  subSim_setJointState = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/SetJointState",100,&Robot::simSetJointStateCallback,this);

  pubSim_getJointState = noeud.advertise<std_msgs::Int32>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/GetJointState",100);
  subSim_getJointState = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/GetJointState",100,&Robot::simGetJointStateCallback,this);

  pubSim_getTime = noeud.advertise<std_msgs::Byte>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/GetTime",100);
  subSim_getTime = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/GetTime",100,&Robot::simGetTimeCallback,this);

  pubSim_getTimeUpdate = noeud.advertise<std_msgs::Byte>("/sim_ros_interface/services/robot"+to_string(num_robot)+"Update/GetTime",100);
  subSim_getTimeUpdate = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"Update/GetTime",100,&Robot::simGetTimeUpdateCallback,this);

  pubSim_changeColor = noeud.advertise<std_msgs::Int32MultiArray>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/ChangeColor",100);
  subSim_changeColor = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/ChangeColor",100,&Robot::simChangeColorCallback,this);

  pubSim_changeShuttleColor = noeud.advertise<std_msgs::Int32MultiArray>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/ChangeShuttleColor",100);
  subSim_changeShuttleColor = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/ChangeShuttleColor",100,&Robot::simChangeShuttleColorCallback,this);

  pubSim_getColor = noeud.advertise<std_msgs::String>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/GetColor",100);
  subSim_getColor = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/GetColor",100,&Robot::simGetColorCallback,this);	

  pubSim_getColorUpdate = noeud.advertise<std_msgs::String>("/sim_ros_interface/services/robot"+to_string(num_robot)+"/GetColorUpdate",100);
  subSim_getColorUpdate = noeud.subscribe("/sim_ros_interface/services/response/robot"+to_string(num_robot)+"/GetColorUpdate",100,&Robot::simGetColorUpdateCallback,this);

  pub_robot_transport=noeud.advertise<std_msgs::Bool>("/commande/Simulation/TransportBras"+to_string(num_robot),10);
  pub_tache_finie=noeud.advertise<aip_cf_commande_locale::TacheFinieMsg>("/commande/Simulation/TacheFinie",10);

  //Subscribers
  planifSendPosition = noeud.subscribe("/commande/Simulation/SendPositionRobot",10,&Robot::SendPositionCallback,this);
  planifSendJoints = noeud.subscribe("/commande/Simulation/SendJointsRobot",10,&Robot::SendJointsCallback,this);
  planifFermerPince = noeud.subscribe("/commande/Simulation/FermerPinceRobot",10,&Robot::FermerPinceCallback,this);
  planifOuvrirPince = noeud.subscribe("/commande/Simulation/OuvrirPinceRobot",10,&Robot::OuvrirPinceCallback,this);
  planifDescendreBras = noeud.subscribe("/commande/Simulation/DescendreBras",10,&Robot::DescendreBrasCallback,this);
  planifMonterBras = noeud.subscribe("/commande/Simulation/MonterBras",10,&Robot::MonterBrasCallback,this);
  planifControlerRobot = noeud.subscribe("/commande/Simulation/ControlerBras",10,&Robot::ControlerRobotCallback,this);
  sub_faireTache = noeud.subscribe("/commande/Simulation/faireTache",10,&Robot::faireTacheCallback,this);
  sub_evacuer=noeud.subscribe("/commande/Simulation/Evacuer",10,&Robot::Evacuer,this);
  subStopTache=noeud.subscribe("/commande/Simulation/Robot"+to_string(num_robot)+"/StopTache",10,&Robot::stopTacheCallback,this);
  subDeplacerPiece=noeud.subscribe("/commande/Simulation/DeplacerPiece",10,&Robot::DeplacerPieceCallback,this);

  //Publishers
  pub_pince = noeud.advertise<std_msgs::Int32>
              ("/robot/cmdPinceRobot"+to_string(num_robot), 10);
  pub_robotPosition = noeud.advertise<std_msgs::Int32>
                      ("/robot/PositionRobot"+to_string(num_robot),10);
  pub_robotBras = noeud.advertise<std_msgs::Int32>
                  ("/robot/BrasRobot"+to_string(num_robot),10);
  pub_robotPince = noeud.advertise<std_msgs::Int32>
                   ("/robot/PinceRobot"+to_string(num_robot),10);
  pub_retourCommande = noeud.advertise<aip_cf_robots::Msg_numrobot>
                       ("/commande/Simulation/retourCommande", 10);

  pub_produitEvac = noeud.advertise<std_msgs::Int32MultiArray>
                    ("/commande/Simulation/produitEvac", 10);

  pub_erreur_log=noeud.advertise<aip_cf_commande_locale::Msg_Erreur>
                 ("/commande/Simulation/Erreur_log",10);

  client = noeud.serviceClient
           <aip_cf_shuttles::shuttle_id>("get_id_shuttle_at_poste");

  ros::Duration(1).sleep();

  // Utilisation du service simRosGetObjectHandle
  // pour obtenir les handles du robot
  for (int i=1;i<8;i++)
  {
    stringstream sr;
    sr << i;
    switch(num_robot){
      case 1:
        msgSim_getObjectHandle.data = "LBR_iiwa_14_R820_joint" + sr.str();
        break;

      case 2:
        msgSim_getObjectHandle.data = "LBR_iiwa_14_R820_joint" + sr.str()+"#0";
        break;

      case 3:
        msgSim_getObjectHandle.data = "LBR_iiwa_14_R820_joint" + sr.str()+"#1";
        break;

      case 4:
        msgSim_getObjectHandle.data = "LBR_iiwa_14_R820_joint" + sr.str()+"#2";
        break;
    }

    pubSim_getObjectHandle.publish(msgSim_getObjectHandle);
    while(!repSim_getObjectHandle&&ros::ok())
    {
      ros::spinOnce();
      loop_rate->sleep() ;
    }
    repSim_getObjectHandle = false;

    Rints[i-1]=valueSim_getObjectHandle;

    if(Rints[i-1]==-1)
    {
      ROS_INFO("Robot %d : Handle non obtenu pour joint %d",num_robot,i);
    }
    else
    {
      if(i==7)
      {
        ROS_INFO("Robot %d OK",num_robot);
        //Retour vers la commande
        retour.data = 0;
        pub_retourCommande.publish(retour);
      }
    }
  }

}

/** Callbacks pour V-Rep **/
void Robot::simGetObjectHandleCallback(const std_msgs::Int32::ConstPtr& msg)
{
  valueSim_getObjectHandle=msg->data;
  repSim_getObjectHandle=true;
}

void Robot::simSetJointStateCallback(const std_msgs::Byte::ConstPtr& msg)
{
  repSim_setJointState=true;
}

void Robot::simGetJointStateCallback(const sensor_msgs::JointState::ConstPtr& msg)
{
  valueSim_getJointState=msg->position[0];
  repSim_getJointState=true;
}

void Robot::simGetTimeCallback(const std_msgs::Float32::ConstPtr& msg)
{
  valueSim_getTime=msg->data;
  repSim_getTime=true;
}

void Robot::simGetTimeUpdateCallback(const std_msgs::Float32::ConstPtr& msg)
{
  valueSim_getTimeUpdate=msg->data;
  repSim_getTimeUpdate=true;
}

void Robot::simChangeColorCallback(const std_msgs::Byte::ConstPtr& msg)
{
  repSim_changeColor=true;
}

void Robot::simChangeShuttleColorCallback(const std_msgs::Byte::ConstPtr& msg)
{
  repSim_changeShuttleColor=true;
}

void Robot::simGetColorCallback(const std_msgs::Int32::ConstPtr& msg)
{
  valueSim_getColor=msg->data;
  repSim_getColor=true;
}

void Robot::simGetColorUpdateCallback(const std_msgs::Int32::ConstPtr& msg)
{
  valueSim_getColorUpdate=msg->data;
  repSim_getColorUpdate=true;
}
