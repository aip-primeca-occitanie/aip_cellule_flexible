#include "FileAttente.h"
#include <ros/ros.h>
using namespace std;

FileAttente::FileAttente(int id_aiguillage, int successeur_droite, int successeur_gauche, queue<int> spec_queue)
{
	id_aiguillage_=id_aiguillage;
	successeur_droite_=successeur_droite;
	successeur_gauche_=successeur_gauche;
	queue_=spec_queue;
}

FileAttente::~FileAttente()
{
}

int FileAttente::maj(int DD, int DG)
{
	//on est là suite au front descendant du capteur concerné
	if (!queue_.empty())
	{
		if (id_aiguillage_==0){return successeur_droite_;}
		//appeler service, pour savoir comment est l'aiguillage
		if(DD)//si l'aiguillage est a droite
		{
			return successeur_droite_;
		}
		else if (DG)//si l'aiguillage est a gauche
		{
			return successeur_gauche_;
		}
		else
		{
			ROS_INFO("J'ai perdu une navette, a partir de maintenant, les deposes de pièces vont faire n'importe quoi");
			ROS_INFO("Vous feriez mieux de relancer la simulation. J'ai perdu une navette car un aiguillage n'était pas en butee droite ou gauche au moment ou une navette c'est engage");
			ros::Duration(3).sleep();
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

int FileAttente::get_first_navette()
{
	return queue_.front();
}

void FileAttente::add_navette_in_queue(int navette)
{
	queue_.push(navette);
}

void FileAttente::delete_navette_in_queue()
{
	if (queue_.empty())
	{
		// Si on est la ça veut dire que le shuttlemanager s'est perdu
		ROS_INFO("Attention, ça va segfault dans 10 secondes, désolé");
		ros::Duration(10).sleep();
	}
	queue_.pop();
}

int FileAttente::get_id_aiguillage()
{
	return id_aiguillage_;
}

queue<int> FileAttente::get_queue()
{
	return queue_;
}
