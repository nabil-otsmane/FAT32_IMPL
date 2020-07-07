#include "disque.h"

extern char* get_nom_disque_physique();
extern unsigned short int Liste_Disques();
extern unsigned short int Lire_Secteur(FILE* Disque_Physique, int Num_Sect, char* secteur);
extern unsigned short int Afficher_Secteur(FILE* Disque_physique, int Num_sect);


int main() 
{
   unsigned short int Lecture_Liste_Disques = Liste_Disques();

    return 0;
}
