#include "disque.h"

//==============================//

char* get_nom_disque_physique() {
    char* nom_disque = (char*)malloc(LEN_NOM_DISQUE*sizeof(char));
    memset(nom_disque, 0, LEN_NOM_DISQUE);
    if (EST_LINUX) {
        strcpy(nom_disque, "/dev/sd");
    } else {
        strcpy(nom_disque, "\\\\.\\PhysicalDrive");
    }
    return nom_disque;
}
//==============================//
unsigned short int Liste_Disques() {
    char* nom_disque = get_nom_disque_physique();
    unsigned short nb_disques = 0;
    while (1) {
        if (EST_LINUX) {
            nom_disque[7] = 'a' + nb_disques;
        } else {
            nom_disque[17] = '0' + nb_disques;
        }
        FILE* disk = fopen(nom_disque, "rb");
        if (disk == NULL) {
            break;
        } else {
            printf("disque %d: %s\n", nb_disques, nom_disque);
            fclose(disk);
        }
        nb_disques++;
    }
    free(nom_disque);
}

//==============================//

unsigned short int Lire_Secteur(FILE* Disque_Physique, int Num_Sect, char* secteur)
{
    if(Disque_Physique == NULL)
        return 1;
    else
    {
        secteur = (char*) malloc (Taille_Secteur*sizeof(char)); 
        fseek(Disque_Physique, Taille_Secteur*Num_Sect - 1, SEEK_SET);
        fread(secteur, Taille_Secteur, 1, Disque_Physique);
        return 0;
    }
}

unsigned short int Afficher_Secteur(FILE* Disque_physique, int Num_sect)
{
    char* sect = NULL;
    unsigned short int lecture_Sect = Lire_Secteur(Disque_physique, Num_sect, sect);
    if(lecture_Sect == 1)
        return 1; 
    else
    {
        long ptr_sect = (long) sect;
        int ligne=0;
        printf("Affichage de contenu du secteur Num: %d, du disque : %s \n", Num_sect, get_nom_disque_physique());
        printf("========================================================\n");
        printf("Adresse  | Contenu(octet de 1 a 16)\n");
        while(ligne < (Taille_Secteur -1))
        {
            printf("%lx   | %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",ptr_sect+ligne, sect[ligne], sect[ligne+1], sect[ligne+2], sect[ligne+3], sect[ligne+4], sect[ligne+5], sect[ligne+6], sect[ligne+7], sect[ligne+8], sect[ligne+9], sect[ligne+10], sect[ligne+11], sect[ligne+12], sect[ligne+13], sect[ligne+14], sect[ligne+15], sect[ligne+16]);
            ligne+=17;
        }
    }
    
}

//==============================//