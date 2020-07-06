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
#include <ctype.h>
#include <stdint.h>

typedef struct {
    uint8_t BS_jmpBoot[3];       // can be either [0xEB, 0x??, 0x90] OR [0xE9, 0x??, 0x??]
	uint8_t BS_OEMName[8];
} Secteur_boot;

uint16_t BPB_BytesPerSector;  // can only be 512, 1024, 2048 or 4096
uint8_t BPB_SectorPerClustor; // must be a power of 2 > 0
uint16_t BPB_ReservedSectorsCount;  // != 0
uint8_t BPB_NumFATs;           // number of fat tables (1 or 2, 2 recommended)
uint16_t BPB_RootEntCnt;       // for FAT32 must be set to 0
uint32_t BPB_FATsz32;	  // count of sectors occupied by one fat
uint32_t BPB_RootClus;	  // the cluster number of the first cluster of the root directory

void Afficher_hex(uint8_t octets[], uint16_t taille) {
    printf("Adresse\t\tContenu (octet de 1 a 16)\n");
    uint16_t i;
    for(i=0;i<taille;i++) {
        if(i % 16 == 0)
            printf("%04x\t\t", i);

        printf("%02x ", octets[i]);

        if(i % 16 == 15)
            printf("\n");
    }
}

FILE* disque_ouvert = NULL;

uint8_t ouvrir_disque(char disque[]) {
    disque_ouvert = fopen(disque, "rb");
    if (disque_ouvert == NULL) {
        printf("le disque n'existe pas.");
        return 0;
    }

    fseek(disque_ouvert, 11, SEEK_SET);
    fread(&BPB_BytesPerSector, 2, 1, disque_ouvert);
    fread(&BPB_SectorPerClustor, 1, 1, disque_ouvert);
    fread(&BPB_ReservedSectorsCount, 2, 1, disque_ouvert);
    fread(&BPB_NumFATs, 1, 1, disque_ouvert);
    fread(&BPB_RootEntCnt, 2, 1, disque_ouvert);

    fseek(disque_ouvert, 36, SEEK_SET);
    fread(&BPB_FATsz32, 4, 1, disque_ouvert);

    fseek(disque_ouvert, 44, SEEK_SET);
    fread(&BPB_RootClus, 4, 1, disque_ouvert);
    return 1;
}

void Afficher_EntetesDisque() {
    if (disque_ouvert == NULL)
        return;
    printf("struct BPB:\n");
	printf("\tBytesPerSector: %d\n", BPB_BytesPerSector);
	printf("\tSectorPerClustor: %d\n", BPB_SectorPerClustor);
	printf("\tReservedSectorsCount: %d\n", BPB_ReservedSectorsCount);
	printf("\tNumFATs: %d\n", BPB_NumFATs);
	printf("\tRootEntCnt: %d\n", BPB_RootEntCnt);
	printf("\tFATsz32: %d\n", BPB_FATsz32);
	printf("\tRootClus: %d\n", BPB_RootClus);
}

uint8_t Liste_Disques() {
    char nom_disque[19];
    memset(nom_disque, 0, 19);
    if (EST_LINUX)
    {
        strcpy(nom_disque, "/dev/sd");
    } else {
        strcpy(nom_disque, "\\\\.\\PhysicalDrive");
    }
    uint8_t nb_disques = 0;
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
    return nb_disques;
}

unsigned char secteur_buff[512]; // buffer du secteur

uint8_t Lire_secteur(char disque_physique[], uint32_t Num_sect) {
    if (ouvrir_disque(disque_physique) == 0) {
        //printf("le disque n'est pas ouvert.");
        return 0;
    }

    Afficher_EntetesDisque();

    uint16_t s = fseek(disque_ouvert, BPB_BytesPerSector*Num_sect, SEEK_SET);
    if (s != 0)
    {
            //printf("\n Erreur de fseek : s= %d",s);
            return 0;
    }
    uint16_t n = fread(secteur_buff, BPB_BytesPerSector, 1, disque_ouvert);
    if (n <= 0) {
        //printf("Erreur de fread = %d\n", n);
        return 0;
    }
    fclose(disque_ouvert);
    disque_ouvert = NULL;
    return 1;
}

void Afficher_secteur(char disque_physique[], uint32_t Num_sect) {
    if (Lire_secteur(disque_physique, Num_sect) == 0) {
        printf("Erreur pandant la lecture de secteur %d du disque %s\n", Num_sect, disque_physique);
        return;
    }

    Afficher_hex(secteur_buff, BPB_BytesPerSector);
}

uint8_t Afficher_Fdel(char disque_physique[], int partition) {
    return 1;
}

int main() {
    /*Liste_Disques();
    Afficher_secteur("fat32.img", 0);*/

    FILE* fp = fopen("/", "rb");
    if (fp == NULL)
    {
        printf("nooo!");
        return 1;
    }


    uint8_t bytes[32];

    fread(bytes, 32, 1, fp);

    Afficher_hex(bytes, 32);

    fclose(fp);

    return 0;
}
