/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>

#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
	// Création de la Socket
	int IdSocket = h_socket(AF_INET, SOCK_STREAM);

	struct sockaddr_in *p_AdrSocket;
	adr_socket(service, serveur, SOCK_STREAM, &p_AdrSocket);

	// Demande de connexion au serveur
	h_connect(IdSocket, p_AdrSocket);
	printf("Bienvenue au jeu du pendu !\n\n");
	printf("Vous devez deviner le mot en entrant des lettres au clavier\n");
	printf("Attention, les minuscules et les majuscules sont considérées comme différentes\n");
	
	// Choix du niveau de difficulté par le client
	// Le niveau de difficulté influe sur le nombre de tentatives
	char lvl;
	do{
		printf("Choisissez la difficulté du jeu : \n");
		lvl = getchar();
	} while ((lvl != '1') && (lvl != '2') && (lvl != '3'));
	// Envoi au serveur
	h_writes(IdSocket, &lvl, 1);

	char tableau[100];
	char taillemotrecu[10];
	int taillemot;
	char entree;
	char retourligne;
	h_reads(IdSocket, taillemotrecu, 1);
	h_reads(IdSocket, tableau, 100);
	taillemot = (int)taillemotrecu[0];
	for (int i = taillemot; i < 2 * taillemot ; i++){
			printf("%c", tableau[i]);
	}
	printf("\n");
	printf("?\n");
	printf("%d tentatives restantes\n", tableau[2 * taillemot]);
	while ((tableau[2 * taillemot] != 0) && (tableau[2 * taillemot + 1] == 0)) {
		do{
			entree = getchar();
		} while ((entree == '\n') || (entree == ' '));
		h_writes(IdSocket, &entree, 1);
		h_reads(IdSocket, tableau, 100);
		for (int i = taillemot; i < 2 * taillemot ; i++){
			printf("%c", tableau[i]);
		}
		printf("\n");
		printf("?\n");
		printf("%d tentatives restantes\n", tableau[2 * taillemot]);
	}
	if (tableau[2 * taillemot + 1]) {
		printf("\n");
		printf("\nGagné !\n");
	} else {
		printf("\nPerdu ! Le mot à deviner était le suivant : \n");
		for (int i = 0; i < taillemot ; i++){
			printf("%c", tableau[i]);
		}
	}
	printf("Déconnexion\n");
	h_close(IdSocket);

}

/*****************************************************************************/

