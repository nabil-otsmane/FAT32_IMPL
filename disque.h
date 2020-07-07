#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//========================//
#ifdef WIN32
#define EST_LINUX 0
#endif // WIN32
#ifdef WINNT
#define EST_LINUX 0
#endif // WINNT

#ifndef EST_LINUX
#define EST_LINUX 1
#endif // EST_LINUX

#define LEN_NOM_DISQUE 20
#define Taille_Secteur 4096

//============================//

char* get_nom_disque_physique();
unsigned short int Liste_Disques();
unsigned short int Lire_Secteur(FILE* Disque_Physique, int Num_Sect, char* secteur);
unsigned short int Afficher_Secteur(FILE* Disque_physique, int Num_sect);
