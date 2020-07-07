#include "disque.h"


int main()
{
    unsigned short int Lecture_Liste_Disques = Liste_Disques();
   if(ouvrir_disque("./bin/Debug/fat32.img")) {
        lireEntete();
   } else {
        return 1;
   }

    Afficher_Secteur(0);

    return 0;
}
