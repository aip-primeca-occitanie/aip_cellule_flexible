/*########################################################################
##  ______                        _____ ___    __    __    ___   ______ ##
## /_  __/__  ____ _____ ___     / ___//   |  / /   / /   /   | / ____/ ##
##  / / / _ \/ __ `/ __ `__ \    \__ \/ /| | / /   / /   / /| |/ / __   ##
## / / /  __/ /_/ / / / / / /   ___/ / ___ |/ /___/ /___/ ___ / /_/ /   ##
##/_/  \___/\__,_/_/ /_/ /_/   /____/_/  |_/_____/_____/_/  |_\____/    ##
######### Steve - Anthony - Lucie - Lucas - Antonin - Guillaume ##########
##########Croce - Favier - Ricart - Veit - Messioux - Auffray-Amen########
##########################################################################
### As a wise man once said : "Follow the white rabbit with hhbbgd...."###
##########################################################################*/
 
#include "capteurs.h"
#include "actionneurs.h"
#include "commande.h"
#include "RobotsInterface.h"
#include "AigsInterface.h"
#include <ros/ros.h>
#include <unistd.h>

using namespace std;

#define RESET   "\033[0m"
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */

#define PlaceFin 1000 /* Marquage à ne pas dépasser */

int M[PlaceFin+1];

// Pour l'affichage //
void display()
{
	cout << endl;
	for (int i=0;i<=PlaceFin;i++)
	{
		if(i==0)
			cout << "Marquage : ";

		if(M[i]>0)
			cout<<BOLDRED<<"M["<<i<<"]="<<M[i]<<RESET<<", ";
		if(M[i]<0)
			cout<<BOLDGREEN<<"M["<<i<<"]="<<M[i]<<RESET<<", ";
	}
	cout<<endl<<endl;
}

void ShutdownCallback(const std_msgs::Byte::ConstPtr& msg)
{
		ros::shutdown();
}

int main(int argc, char **argv)
{
	////////////////////////////////////////////////////
	///////////  | Debut du Petri plus bas |  //////////
	////////////////////////////////////////////////////


	ros::init(argc, argv, "commande");
	ros::NodeHandle noeud;

	ros::Publisher pub_spawnShuttles = noeud.advertise<std_msgs::Int32>("/commande_locale/nbNavettes",10);
	ros::Subscriber sub_shutdown = noeud.subscribe("/commande_locale/shutdown",10,&ShutdownCallback);

	int nbRobot=atoi(argv[1]);

	Commande cmd(noeud,argv[0]);
	RobotsInterface robot(noeud,nbRobot);
	AigsInterface aiguillage(noeud);
	Capteurs capteur(noeud);

	ros::Rate loop_rate(25); //fréquence de la boucle

	// On attend la fin de l'initialisation des robots
	while(!robot.RobotInitialise(1) || !robot.RobotInitialise(2))
	{
		ros::spinOnce();
		loop_rate.sleep();
	}	
	while(nbRobot==4 && (!robot.RobotInitialise(3) || !robot.RobotInitialise(4)))
	{
		ros::spinOnce();
		loop_rate.sleep();
	}

	///////////////////////////////
	// | Creation des Navettes | //
	///////////////////////////////

	int nbNavettes=2;//Mettre 0 pour demander a l'utilisateur
	while(nbNavettes<1||nbNavettes>6)
	{
		cout << "Combien voulez vous de navettes ? [1..6]" << endl;
		cin >> nbNavettes;
		if(cin.fail())
		{
			cout << endl << " [Erreur mauvais choix ..]" << endl;
			cin.clear();
			cin.ignore(256,'\n');
		}
	}
	std_msgs::Int32 msg_nbNavettes;
	msg_nbNavettes.data=nbNavettes;
	pub_spawnShuttles.publish(msg_nbNavettes);
	int code_produit_a_ajouter=-1;

	cmd.Initialisation();
	for(int i=0;i<PlaceFin;i++) M[i]=0;

	////////////////////////////////////
	////// | MARQUAGE INITIAL | ////////
	////////////////////////////////////
	M[0]=1;
	display();

	while (ros::ok())
	{
		// Seulement si la simulation est en cours
		if(cmd.getPlay()==true)
		{
			/////////////////////////////////
			/// | GESTION AJOUT PRODUIT | ///
			/////////////////////////////////

			// Code_produit : dizaine=poste, unité=produit
			// Ex : code=24 => Produit 4 sur poste 2
			code_produit_a_ajouter=robot.AjoutProduitEnAttente();
			while(code_produit_a_ajouter!=-1) // Tant qu'il reste des produits à ajouter
			{
				M[500+code_produit_a_ajouter]++;
				code_produit_a_ajouter=robot.AjoutProduitEnAttente();
				display();
			}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// | DEBUT PETRI | ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if (M[0]) // apparaitre produit B sur poste 3
			{
				M[0]--;
				robot.AjouterProduit(POSTE_3,2); // ajout produit n°2 (donc B) sur poste 3
				robot.AjouterProduit(POSTE_7,6);
				cmd.Stop_PS(10);
				M[10]++;
				display();
			}

			if (M[10] && capteur.get_PS(1)) // quand navette à proximité du poste 3, on le fait arrêter au niveau du poste
			{
				M[10]--;
				M[532]--; // Enleve le marquage des produits
				M[576]--; // Enleve le marquage des produits
				cmd.Stop_PS(2);
				robot.FaireTache(POSTE_7,3);
				M[20]++;
				display();
			}

			if (M[20] && capteur.get_PS(2) && robot.TacheFinie(POSTE_7)) // le robot 2 prend le produit B sur le poste 3 et le met sur la navette
			{
				M[20]--;
				robot.DeplacerPiece(ROBOT_2,1,2);
				robot.DeplacerPiece(ROBOT_4,1,3);
				cmd.SortirErgot(1);
				M[30]++;
				display();
			}

			if (M[30]  && robot.FinDeplacerPiece(ROBOT_2) && robot.FinDeplacerPiece(ROBOT_4)) // la navette repars du poste 3 avec le produit B
			{
				M[30]--;
				cmd.Ouvrir_PS(2);
				cmd.Ouvrir_PS(10);
				aiguillage.Droite(2);
				cmd.Stop_PS(6);
				cmd.Stop_PS(14);
				M[40]++;
				display();
			}

			if (M[40] && capteur.get_PS(6) && capteur.get_PS(14))//On dirige la navette vers le poste 1
			{
				M[40]--;
				robot.DeplacerPiece(ROBOT_3,2,1);
				aiguillage.Gauche(3);
				aiguillage.Gauche(10); // on le met ici car pas de capteur de position entre aiguillage 3 et 10
				M[50]++;
				display();
			}

			if (M[50] && capteur.get_DG(3) && capteur.get_DG(10) && robot.FinDeplacerPiece(ROBOT_3))// quand les aiguillages ont fini de tourner on fait partir la navette
			{
				M[50]--;
				cmd.Ouvrir_PS(6);
				cmd.Stop_PS(22);
				robot.FaireTache(POSTE_6,6);
				M[60]++;
				display();
			}

			if (M[60] && capteur.get_PS(20)) // Attend front descendant de PS20
			{
				M[60]--;
				M[70]++;
				display();
			}

			if(M[70] && !capteur.get_PS(20)) // Front descendant : la navette 1 est passée, on bouge les aiguillages pour la navette 0
			{
				M[70]--;
				aiguillage.Droite(10);
				aiguillage.Droite(3);
				M[80]++;
				display();
			}

			if (M[80] && capteur.get_PS(22) && robot.TacheFinie(POSTE_6)) // Une fois l'aiguillage 10 lock on laisse passer la navette 0
			{
				M[80]--;
				robot.DeplacerPiece(ROBOT_3,1,4);
				robot.DeplacerPiece(ROBOT_1,3,4);
				cmd.SortirErgot(8);
				M[90]++;
				display();
			}

			if (M[90] && robot.FinDeplacerPiece(ROBOT_1) && robot.FinDeplacerPiece(ROBOT_3)) // le robot 1 prend le produit B sur la navette et le met sur le poste 1
			{
				M[90]--;
				robot.FaireTache(POSTE_5,3);
				robot.FaireTache(POSTE_1,4);
				M[100]++;
				display();
			}

			if (M[100] && robot.TacheFinie(POSTE_5)) // robot 1 fais tache 1 pendant 4s
			{
				M[100]--;
				robot.FaireTache(POSTE_5,1);
				cmd.Ouvrir_PS(14);
				cmd.Stop_PS(15);
				M[110]++;
				display();
			}

			if (M[110] && robot.TacheFinie(POSTE_1) && robot.TacheFinie(POSTE_5)) // le robot 1 prend le produit B sur le poste et le met sur la navette quand tache fini
			{
				M[110]--;
				robot.DeplacerPiece(ROBOT_1,4,3);
				robot.DeplacerPiece(ROBOT_3,4,3);
				M[120]++;
				display();
			}

			if (M[120] && robot.FinDeplacerPiece(ROBOT_1) && robot.FinDeplacerPiece(ROBOT_3)) // la navette repars du poste 1 avec le produit B qui a fait la tâche 1
			{
				M[120]--;
				cmd.Ouvrir_PS(22);
				cmd.Ouvrir_PS(15);
				M[130]++;
				display();
			}

			if(M[130] && capteur.get_PS(1))
			{
				M[130]--;
				aiguillage.Gauche(11);
				aiguillage.Gauche(12);
				M[140]++;
				display();
			}

			if (M[140] && capteur.get_CP(1)) // quand la navette arrive à proximité du poste 4, on le fait arrêter au niveau du poste
			{
				M[140]--;
				cmd.Stop_PS(3);
				M[141]++;
				M[150]++;
				display();
			}

			if(M[141] && capteur.get_PS(2))
			{
				M[141]--;
				M[142]++;
				display();
			}

			if(M[142] && !capteur.get_PS(2))
			{
				M[142]--;
				cmd.Stop_PS(2);
				display();
			}

			if (M[150] && capteur.get_PS(3)) // le robot 2 prend le produit B sur la navette et le met sur le poste 4
			{
				M[150]--;
				robot.DeplacerPiece(ROBOT_2,3,4);
				M[160]++;
				display();
			}

			if (M[160] && robot.FinDeplacerPiece(ROBOT_2)) // On fait la tache du poste 4 pendant 5s
			{
				M[160]--;
				robot.FaireTache(POSTE_4,5);
				M[170]++;
				display();
			}

			if(M[170] && capteur.get_PS(2))
			{
				M[170]--;
				robot.DeplacerPiece(ROBOT_2,2,1);
				M[180]++;
				display();
			}

			if(M[180] && robot.FinDeplacerPiece(ROBOT_2))
			{
				M[180]--;
				robot.FaireTache(POSTE_3,1);
				M[182]++;
				display();
			}
			if(M[182] && robot.TacheFinie(POSTE_3))
			{
				M[182]--;
				robot.Evacuer();
				M[190]++;
				display();
			}

			if (M[190] && robot.TacheFinie(POSTE_4)) // le robot 2 prend le reproduit B sur le poste et le met sur le poste 3
			{
				M[190]--;
				robot.DeplacerPiece(ROBOT_2,4,1);
				M[200]++;
				display();
			}

			if (M[200] && robot.FinDeplacerPiece(ROBOT_2)) // On évacue le produit final et redémarre la navette
			{
				M[200]--;
				robot.Evacuer(); // Evacue le produit
				cmd.Ouvrir_PS(3); // la navette repart
				cmd.Ouvrir_PS(2);
				aiguillage.Gauche(3);
				aiguillage.Gauche(10);
				M[210]++;
				display();
			}

			if(M[210] && capteur.get_PS(6))
			{
				M[210]--;
				M[220]++;
				display();
			}

			if(M[220] && !capteur.get_PS(6))
			{
				M[220]--;
				M[230]++;
				display();
			}

			if(M[230] && capteur.get_PS(6))
			{
				M[230]--;
				M[240]++;
				display();
			}

			if(M[240] && !capteur.get_PS(6))
			{
				M[240]--;
				aiguillage.Gauche(1);
				aiguillage.Gauche(2);
				M[PlaceFin]++;
				display();
			}

			///////////////////////////////
			// | Place de fin de Petri | //
			///////////////////////////////

			if(M[PlaceFin])
			{
				display();
				cout << endl << BOLDCYAN << " --[PETRI TERMINE]--" << RESET << endl;
				cmd.FinPetri();
				while(ros::ok())
				{
					ros::spinOnce();
					loop_rate.sleep();
				}
			}
		}

		ros::spinOnce(); //permet aux fonction callback de ros dans les objets d'êtres appelées
		loop_rate.sleep(); //permet de synchroniser la boucle while. Il attend le temps qu'il reste pour faire le 25Hz (ou la fréquence indiquée dans le loop_rate)
	}

	return 0;
}
