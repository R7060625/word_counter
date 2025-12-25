#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define nonwords " ,.;:!?\n\t"

/*PREMIER ALGORITHME : 
    rangement itératif des mots; Dès qu'un nouveau mot est rencontré,
    on le rajoute à notre liste dynamique avec son nombre d'itérations. 
    On trie ensuite la liste de manière décroissante afin que les mots soient
    rangés dans l'ordre décroissant. 
    Complexité : O(n²) pour le rangement des mots, 
                 O(n log(n)) pour le tri de la liste
*/

typedef struct {
    int* cumul_alloc; // liste dynamique de chaînes
    int*  cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    int* max_alloc;
} InfoMem;

typedef struct {
    int iteration;
    char* mot[];
} Mot;

int delimite_mot(char** chaine, FILE* fichier, char char_debut){

    chaine[0] = &char_debut;
    int index = 1;

    char c;

    while(!strchr(nonwords, (c = fgetc(fichier)))) {
        chaine[index] = &c;
        index++;
    }

    return 1;
}   

int compteMots_algo1(FILE *fichier, Mot* liste_iter) {

    int compteur = 1;

    int c;
    while ((c = fgetc(fichier)) != EOF) {

        char chaine[50];

        /*// on tombe sur un séparateur
        if (strchr(nonwords, c)) {
            
            c = fgetc(fichier);

            // tant qu'il y a un separateur, on prend le nouveau caractere
            while (strchr(nonwords, c)) {
                c = fgetc(fichier);
            };

            compteur++;
        };*/

        if (!strchr(nonwords, c)) { // lettre valide 

            // delimite_mot

        }



    }
    return compteur;
}



int main(int argc, char* argv[]) {

    Mot* liste_iter = (Mot *) malloc(1000 * sizeof(Mot)); // On commence par ajouter 1000 emplacements de mots dans la liste 

    FILE *file = fopen(argv[1], "r");
        printf("Il y a %d mots dans le fichier %s\n", compteMots_algo1(file, liste_iter), argv[1]);

        /*char string[500];
        assigneLigne(file, string);

        printf("La ligne est %s", string );*/
        fclose(file);


    return 0;
}
