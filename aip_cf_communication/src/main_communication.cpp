/**** Bruno DATO, Abdellah ELGOURAIN, Evgeny SHULGA M1 EEA ISTR Université Paul Sabatier Toulouse III 2016 ****/

#include "communication_API_schneider.h"
#include "communication_commande.h"
#include <ros/ros.h> 

using namespace std;

int main(int argc, char **argv)
{

	ros::init(argc, argv, "communication");
	ros::NodeHandle noeud;

	Communication_API_schneider AP1(noeud,"1"),AP2(noeud,"2");
	Communication_commande Com(noeud);

	ros::Rate loop_rate(27);
	
	while (ros::ok()) // Tant que ROS est actif,
	{
		Com.Concatenation_entrees(AP1,AP2);
		
		Com.publish();

		Com.Decoupe_sorties(&AP1,&AP2);
		
		cout << "Sorties AP1 = " 	<<AP1.sorties_api << endl;
		cout << "Sorties AP2 = "	<<AP2.sorties_api << endl;

		AP1.publish();
		AP2.publish();
		
		ros::spinOnce(); // on lance les callback correspondant aux messages entrants.

		loop_rate.sleep();
	}
 
	return 0;
}
