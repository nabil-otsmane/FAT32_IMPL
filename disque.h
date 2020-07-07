#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

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

typedef struct {
    uint8_t BS_jmpBoot[3];       // can be either [0xEB, 0x??, 0x90] OR [0xE9, 0x??, 0x??]
	uint8_t BS_OEMName[8];
} Secteur_boot;

typedef struct Fichier {
	char file_name[12];
	uint32_t taille;
	uint32_t attr;
	uint32_t num_premier_clus;
	uint32_t mode;
} Fichier;

typedef struct Repertoire {
	Fichier fichiers[64];
	uint32_t num_files;
} Repertoire;

uint8_t ouvrir_disque(char disque[]);
char* get_nom_disque_physique();
uint8_t Liste_Disques();
uint8_t Lire_Secteur(uint32_t Num_sect);
unsigned short int Afficher_Secteur(int Num_sect);
void lireEntete();
void Afficher_hex(uint8_t octets[], uint16_t taille);
