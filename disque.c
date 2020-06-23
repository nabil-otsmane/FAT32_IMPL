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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
