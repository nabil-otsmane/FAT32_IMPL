#include <stdio.h>
#include <stdlib.h>

extern void Liste_Disques();
extern void Lire_secteur(FILE* disque_physique, unsigned int Num_sect);
extern void Afficher_secteur(FILE* disque_physique, unsigned int Num_sect);
extern void Afficher_Fdel(FILE*
                           disque_physique, int partition);

int main() {
    printf("test");

    return 0;
}
