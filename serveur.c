/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <curses.h>
#include <string.h>
#include <time.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}

//------GESTION DU JEU----------------------------------------------------------

void srand(unsigned int seed);//Initialise la graine

void mot_alea(char** tab, int len, char * tableau, int * taillemot){ //Choisit un mot aleatoirement dans le tableau et l'affecte à la structure
  int indice = rand() % len;
  char* motchoisi = tab[indice];
  int taille = strlen(motchoisi);
  for (int i = 0; i<taille ; i++) {
	tableau[i] = motchoisi[i];
	tableau[i + taille] = '-';
  }
  tableau[2 * taille + 1] = 0; // indicateur de partie gagnante (si le mot est deviné)
  *taillemot = taille;
}

void modif_copie(char * tableau, char caract, int taillemot){//Devoile les lettres de la copie en fonction des entrees du joueur
  bool found = false;
  int indice = 0;
  while (indice != taillemot){
    if (((tableau[indice] == caract) || (tableau[indice] - 32 == caract) || (tableau[indice] + 32 == caract)) && (tableau[indice + taillemot] == '-')) {//Si une lettre du word a deviner est egale au word du joueur
      tableau[indice + taillemot] = tableau[indice];//on remplace dans la copie
	  found = true;
    }
    indice++;
  }
  if (found){
	bool trouve = true;
	for (int i = 0; i < taillemot; i++){
		if (tableau[i] != tableau[i + taillemot]){
			trouve = false;
		}
	}
	if (trouve){
		tableau[2 * taillemot + 1] = 1; // si le mot est trouvé, on set le caractère à 1
	}
	return;
  }
  tableau[2 * taillemot]--;
  return;
}


/******************************************************************************/	
void serveur_appli(char *service)
{

	/* Procedure correspondant au traitemnt du serveur de votre application */

	// Création de la socket côté serveur
	int IdSocket = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in *p_AdrSocketServeur;
	adr_socket(service, NULL, SOCK_STREAM, &p_AdrSocketServeur);
	h_bind(IdSocket, p_AdrSocketServeur);
	// La socket écoute les demandes du client,h_reads(IdClient, rcvBuf, 2);le nombre de demandes maximales est de 1
	h_listen(IdSocket, 1);
	// On accepte la demande de connexion du client
	struct sockaddr_in * p_AdrSocketClient;
	while (1){
		int IdClient = h_accept(IdSocket, p_AdrSocketClient);

		int pid;
		pid = fork();

		// Si c'est un processus fils
		if(pid == 0){

			h_close(IdSocket);

			srand(clock());//Initialise le générateur aleatoire
			
			char rcvBuf[100];
			char tableau[100];
			int taillemot;
			int nbtentatives;

			

			// Initialisation du dictionnaire utilisé
			char* dico[4];
			dico[0] = "PingPong";
			dico[1] = "Kayak";
			dico[2] = "Rugby";
			dico[3] = "Football";
			
			mot_alea(dico, 4, tableau, &taillemot);//Tirage aleatoire du mot dans le dico

			// Le client entre la difficulté du jeu
			h_reads(IdClient, rcvBuf, 1);//level, 1);
			char lvl = rcvBuf[0];
			
			switch(lvl){
				case '1':
					nbtentatives = taillemot + 5;
					break;
				case '2':
					nbtentatives = taillemot + 2;
					break;
				case '3':
					nbtentatives = taillemot;
					break;
				default:
					nbtentatives = 0;
					break;
			}
			tableau[2 * taillemot] = nbtentatives; // indicateur du nombre de tentatives restantes
			rcvBuf[0] = taillemot;
			printf("Taille = %d\n", rcvBuf[0]);
			h_writes(IdClient, rcvBuf, 1);
			h_writes(IdClient, tableau, 100);
			printf("Mot envoyé !\n");
			char reponse;
			while (tableau[2 * taillemot] != 0){
				h_reads(IdClient, rcvBuf, 1);
				reponse = rcvBuf[0];
				printf("Reception = %d %d\n", rcvBuf[0], rcvBuf[1]);
				if (reponse != 10){ // On ignore les retours à la ligne
					modif_copie(tableau, reponse, taillemot);//On ajuste le mot forme de tirets '-'
				}
				h_writes(IdClient, tableau, 100);
			}
			h_close(IdClient);
		} else {
			h_close(IdClient);
		}
	}
}

/******************************************************************************/	

