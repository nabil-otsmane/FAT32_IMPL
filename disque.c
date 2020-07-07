#include "disque.h"

uint16_t BPB_BytesPerSector;  // can only be 512, 1024, 2048 or 4096
uint8_t BPB_SectorPerClustor; // must be a power of 2 > 0
uint16_t BPB_ReservedSectorsCount;  // != 0
uint8_t BPB_NumFATs;           // number of fat tables (1 or 2, 2 recommended)
uint32_t BPB_FATsz32;	  // count of sectors occupied by one fat
uint32_t BPB_RootClus;	  // the cluster number of the first cluster of the root directory
uint32_t prem_secteur_data;

FILE* fp;

FILE* disque_ouvert = NULL;

unsigned char secteur_buff[512]; // buffer du secteur

uint32_t rep_courr = 0;	        // numero cluster du repertoire courrant
Repertoire current_directory;	// l'object du repertoire courrant
Repertoire open_files;			// The Directory object for the open files
uint32_t max_open_files = 64;	    // The maximum index of the open files

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

//==============================//

unsigned short int Afficher_Secteur(int Num_sect)
{
    unsigned short int lecture_Sect = Lire_Secteur(Num_sect);
    if(lecture_Sect == 0)
        return 1;
    else
    {

        printf("Affichage de contenu du secteur Num: %d, du disque : %s \n", Num_sect, get_nom_disque_physique());
        printf("========================================================\n");
        Afficher_hex(secteur_buff, BPB_BytesPerSector);
    }

    return 0;

}



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

// lire un entier selon l'ordre "little-endian"
uint32_t LireEntier(uint32_t pos, uint32_t taille){
	uint32_t data = 0, temp;

	pos--;

	while(taille > 0){
		fseek(disque_ouvert, pos, SEEK_SET);
		fseek(disque_ouvert, taille, SEEK_CUR);
		taille--;
		temp = fgetc(fp);
		data = data | temp;
		if(taille != 0)
			data = data << 8;
		//printf("DATA:\t%in", data);
	}
	return data;
}



uint8_t ouvrir_disque(char disque[]) {
    disque_ouvert = fopen(disque, "rb");

    if (disque_ouvert == NULL) {
        return 0;
    }

    return 1;
}

void lireEntete() {

    fseek(disque_ouvert, 11, SEEK_SET);
    fread(&BPB_BytesPerSector, 2, 1, disque_ouvert);
    fseek(disque_ouvert, 13, SEEK_SET);
    fread(&BPB_SectorPerClustor, 1, 1, disque_ouvert);
    fseek(disque_ouvert, 14, SEEK_SET);
    fread(&BPB_ReservedSectorsCount, 2, 1, disque_ouvert);
    fseek(disque_ouvert, 16, SEEK_SET);
    fread(&BPB_NumFATs, 1, 1, disque_ouvert);
    fseek(disque_ouvert, 36, SEEK_SET);
    fread(&BPB_FATsz32, 4, 1, disque_ouvert);
    fseek(disque_ouvert, 44, SEEK_SET);
    fread(&BPB_RootClus, 4, 1, disque_ouvert);

    prem_secteur_data = BPB_ReservedSectorsCount + (BPB_NumFATs * BPB_FATsz32);
}

void Afficher_EntetesDisque() {
    if (disque_ouvert == NULL)
        return;
    printf("struct BPB:\n");
	printf("\tBytesPerSector: %d\n", BPB_BytesPerSector);
	printf("\tSectorPerClustor: %d\n", BPB_SectorPerClustor);
	printf("\tReservedSectorsCount: %d\n", BPB_ReservedSectorsCount);
	printf("\tNumFATs: %d\n", BPB_NumFATs);
	printf("\tFATsz32: %d\n", BPB_FATsz32);
	printf("\tRootClus: %d\n", BPB_RootClus);
}



uint8_t Lire_Secteur(uint32_t Num_sect) {

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
    return 1;
}

Repertoire getContenuRepertoire(uint32_t num_clus){
	Repertoire contenu;
	uint32_t dir_fsc;
	uint32_t dir_adr;
	uint32_t dir_clus_hi_word;
	uint32_t dir_clus_lo_word;
	uint32_t dir_clus;
    char dir_name[12];
	unsigned char dir_attr;
	unsigned int i, j, c = 0, dir_size;
	char ch;
	// Locates the first sector of the root directory cluster;
	dir_fsc = ((num_clus - 2) * BPB_SectorPerClustor) + prem_secteur_data;
	dir_adr = dir_fsc * (BPB_BytesPerSector * BPB_SectorPerClustor);

	for(i = 0; i < (BPB_BytesPerSector*BPB_SectorPerClustor); i+=32){
		Fichier temp_file;
		// Extract Attributes
		//dir_attr = LireEntier(dir_adr+i+11, 1);
		fseek(disque_ouvert, dir_adr+i+11, SEEK_SET);
		fread(&dir_attr, 1, 1, disque_ouvert);
		// Continues on long-name entries.
		if(dir_attr == 15)
			continue;

		// Extracts File name - max 11 chars
		for(j = 0; j < 11; j++){

			//ch = LireEntier(dir_adr+i+j, 1);
			fseek(disque_ouvert, dir_adr+i+j, SEEK_SET);
			fread(&ch, 1, 1, disque_ouvert);

			if(ch != 0)
				dir_name[j] = ch;
			else
				dir_name[j] = ' ';
		}
		dir_name[11] = 0;
		if(dir_name[0] == ' ')
			continue;

		// Extracts File Size
		//dir_size = LireEntier(dir_adr+i+28, 4);
		fseek(disque_ouvert, dir_adr+i+28, SEEK_SET);
		fread(&dir_size, 4, 1, disque_ouvert);

		//dir_clus_hi_word = LireEntier(dir_adr+i+20, 2);
		fseek(disque_ouvert, dir_adr+i+20, SEEK_SET);
		fread(&dir_clus_hi_word, 2, 1, disque_ouvert);
		//dir_clus_lo_word = LireEntier(dir_adr+i+26, 2);
		fseek(disque_ouvert, dir_adr+i+26, SEEK_SET);
		fread(&dir_clus_lo_word, 2, 1, disque_ouvert);

		dir_clus = dir_clus_hi_word | dir_clus_lo_word;

		strcpy(temp_file.file_name, dir_name);
		temp_file.taille = dir_size;
		temp_file.attr = dir_attr;
		temp_file.num_premier_clus = dir_clus;
		contenu.fichiers[c++] = temp_file;
	}
	contenu.num_files = c;
	return contenu;
}

uint8_t Afficher_Fdel() {
    int i;
    Repertoire dir;
    Fichier* next_dir;
    char dir_name[12];

    rep_courr = BPB_RootClus;
    current_directory = getContenuRepertoire(rep_courr);

    printf("test\n");

    fflush(fp);

    if (current_directory.num_files == 0) {
        fprintf(stderr, "There are no files in the current directory.\n");
		return 0;
	}

	if(rep_courr = BPB_RootClus && strcmp(dir_name, ".") == 0){
		PrintOpenFiles(current_directory);
		return 1;
	}
	/*next_dir = SearchForFileInCurrentDirectory(dir_name);
	if(next_dir == NULL){
		fprintf(stderr,
			"ERROR: '%s' could not be found in the current directory.\n",
			 dir_name);
		return 0;
	}
	else if(!CheckBitSet(next_dir->file_attr, 4)){
		fprintf(stderr, "ERROR: '%s' is not a directory.\n", dir_name);
		return 0;
	}*/
	PrintOpenFiles(getContenuRepertoire(next_dir->num_premier_clus));

}

void PrintOpenFiles(Repertoire dir){
	int i;

	if(dir.num_files == 0){
		printf("There are no open files.\n");
		return;
	}
	puts("================================");
	for(i = 0; i < max_open_files; i++){
		Fichier* temp = &dir.fichiers[i];
		if(temp->file_name[0] == '\0'){
			continue;
		}
		/*if(!CheckBitSet(temp->attr, 4))
			printf("%s: ", "F");
		else
			printf("%s: ", "D");*/
		printf("%s - ", temp->file_name);
		printf("%i bytes", temp->taille);
		char* md;
		switch(temp->mode){
		case 1: md = "r"; break;
		case 2: md = "w"; break;
		case 3: md = "rw"; break;
		default: md = "error"; break;
		}
		printf(", in mode '%s'\n", md);
	}
	puts("================================");
}

//==============================//
