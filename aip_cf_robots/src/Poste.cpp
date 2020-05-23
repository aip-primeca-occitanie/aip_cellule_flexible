/**** Projet long N7 2020 ****/
#include "Poste.h"

//Constructeur
Poste::Poste()
{
	duree_task_=0;
	temps_debut_task_=0;
	task_en_cours_=false;
}

//Destructeur
Poste::~Poste()
{
}


void Poste::init(string nom, int numero)
{
	nom_poste_=nom;
	numero_poste_=numero;
	couleur_=numero_poste_*10+3;
}

//le robot ne doit pas être occupé pendant la tâche

string Poste::get_nom()
{
	return nom_poste_;
}

int Poste::get_color()
{
	return couleur_;
}

int Poste::get_numero()
{
	return numero_poste_;
}

int Poste::get_duree()
{
	return duree_task_;
}


void Poste::debutTask(float VREPtime, int duree)
{
	task_en_cours_=true;
	temps_debut_task_=VREPtime;
	duree_task_=duree;
}

bool Poste::updateTask(float time)
{
	bool fin_tache=false;
	float restant=(temps_debut_task_+duree_task_)-time;
	if(restant<0)
		restant=0;

	cout << "Poste N=" << numero_poste_ << " task_en_cours=" << task_en_cours_ << fixed << setprecision(1) << " Temps restant=" << restant << " duree=" << duree_task_ << endl;

	if(task_en_cours_ && time>temps_debut_task_+duree_task_)
	{
		task_en_cours_=false;
		fin_tache=true;
	}

	return fin_tache;
}

bool Poste::isTaskEnCours()
{
	return task_en_cours_;
}

void Poste::stopTask()
{
	task_en_cours_=false;
}
