#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

//------GESTION DES ENTREES DE L'UTILISATEUR------------------------------------ 

#define STDIN   0  // Standard input (stdin)
#define STDOUT  1  // Standard output (stdout)
#define STDERR  2  // Standard error (stderr)


int read_char(char* ch) { //Lire un caractère dans l'entrée standard
  return read(STDIN, ch, 1);//Retourne le nombre de caractères lus ou -1 en cas d'erreur
}

#define BUFFER_SIZE 80 //Buffer pour lire les caractères entrés au clavier (80 octets)
char* buffer=NULL;

char* read_line() {//Lire la ligne entrée au clavier, s'arrête lorsqu'on entre "ENTER"
  int len = 0;
  char *str;
  char ch;
  do {
    if (len >= BUFFER_SIZE)//Dans le cas d'un dépassement de buffer, on sort de la boucle
      break;
    if (read_char(&ch)!=1) {//Si on ne lit pas un caractère, on sort de la boucle
      if (len==0)
        return NULL;
      break;
    }
    buffer[len] = ch;//On ajoute le caractère au buffer
    len = len + 1;
  } while (ch!='\n');//Caractère signifiant une fin de ligne
  len = len - 1;
  buffer[len]='\0';//On remplace '\n' par '\0' (fin de chaîne de caractère
  str = malloc(len+1); //Sauvegarde de la chaîne pour réutiliser le buffer (à la ligne suivante)
  for (int i=0;i<=len;i++)
    str[i] = buffer[i];
  return str;
}

//------GESTION DU JEU----------------------------------------------------------

char** allouer_tab(){//Alloue la memoire pour le tableau (rempli de 0 en fin de fonction)
  char** dico;
  int size = 4 * sizeof(char*);
  dico = malloc(size);
  memset(dico,0,size);//dico est rempli de 0
  return dico;
}

void ajouter_mot(char** tab, int indice, char* copie){//Ajoute les mots dans le tableau
  tab[indice]=copie;
  return;
}

void srand(unsigned int seed);//Initialise la graine

char* mot_alea(char** tab, int len){//Choisit un mot aleatoirement dans le tableau
  int indice = rand() % len;
  char* mot = tab[indice];
  return mot; 
}

char* creation_copie(char* mot){//Cree le mot consitue de '-' en debut de partie
  int taille = strlen(mot);
  char* copie = malloc(taille);//On n'oublie pas le caractère de fin de chaîne
  for (int i=0;i<taille;i++){
    copie[i] = '-';
  }
  copie[taille] = 0;
  return copie; 
}

void modif_copie(char* mot, char* copie, char* caract){//Devoile les lettres de la copie en fonction des entrees du joueur
  int taille = strlen(mot);
  int indice = 0;
  while (indice != taille){
    if ((mot[indice] == *caract) || (mot[indice] - 32 == *caract) || (mot[indice] + 32 == *caract)) {//Si une lettre du mot a deviner est egale au mot du joueur
      copie[indice] = mot[indice];//on remplace dans la copie
    }
    indice++;
  }
}

int verif_gagne(char* mot, char* copie){//Verification de la victoire
  int gagne = 1;
  int taille = strlen(mot);
  for (int i=0; i<taille;i++){
    if (mot[i] != copie[i]) gagne=0;//Si le mot cache n'est pas egal au mot a trouver, la partie est perdue
  }
  return gagne;
}

//------LE PROGRAMME------------------------------------------------------------

int main(int nargs, char** args) {

  buffer = malloc(BUFFER_SIZE);
  setbuf(stdout, NULL);

  srand(clock());//Initialise le générateur aleatoire
  char** dico = allouer_tab();//Le tableau vide
  ajouter_mot(dico,0,"Woody");//Ajout des mots au tableau
  ajouter_mot(dico,1,"Andy");
  ajouter_mot(dico,2,"Buzz");
  ajouter_mot(dico,3,"Tom");
  int joue=1;//Condition de jeu
  printf("Bienvenue au jeu du pendu !\n\n");
  printf("Vous devez deviner le mot en entrant des lettres au clavier\n");
  printf("Attention, les minuscules et les majuscules sont considérées comme différentes\n");
  while (joue){
    char* mot = mot_alea(dico,4);//Tirage aleatoire du mot dans le dico
    int points = strlen(mot) * 2;
    printf("Vous avez %d coups pour deviner le mot\n", points);
    char* copie = creation_copie(mot);//La copie est de meme taille que le mot a deviner, forme de tirets uniquement
    while (1){
      printf("%s\n", copie);
      printf("?\n");
      int verif = 0;
      while (verif != 1){//Tant que l'entree n'est pas un caractere, on redemande une entree
        char *caract = read_line();
        if (!((strlen(caract) == 1) && (((*caract >= 'a') && (*caract <= 'z')) || ((*caract >= 'A') && (*caract <= 'Z'))))){
          printf("Entrez un caractere\n");
          printf("?\n");
        }else{
          modif_copie(mot,copie,caract);//On ajuste le mot forme de tirets '-'
          free(caract);
          verif = 1;
        }
      }
      if (verif_gagne(mot,copie) == 1) break;
      points--;
      if (points == 0) break;
    }
    printf("%s\n", copie); 
    if (verif_gagne(mot,copie)) printf("Vous avez gagné ! Voulez vous rejouer (o/n) ?\n");
    else printf("Vous avez perdu ... Voulez vous rejouer (o/n) ?\n");
    free(copie);
    int verif = 0;
    while (verif != 1){//Tant que l'entree n'est pas 'o' ou 'n', on redemande une entree
      char *caract2 = read_line();
      if ((*caract2 != 'o') && (*caract2 !='n')){
        printf("Entrez o ou n\n");
      }else{
        verif = 1;
        if (*caract2 =='n') joue = 0;
        free(caract2);
      }
    }
    
  }
  free(dico);//On libere la memoire alloue pour le tableau et les doublons
  printf("Merci d'avoir joué, au revoir\n");
}
