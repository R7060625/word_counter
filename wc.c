#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define nonwords " ,.;:!?\n\t"

/*PREMIER ALGORITHME : 
    rangement itératif des mots; Dès qu'un nouveau mot est rencontré,
    on le rajoute à notre liste dynamique avec son nombre d'itérations. 
    On trie ensuite la liste de manière décroissante afin que les mots soient
    rangés dans l'ordre décroissant. 
    Complexité : O(n²) pour le rangemen#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define nonwords " ,.;:!?\n\t"

/*PREMIER ALGORITHME : 
    rangement itératif des mots; Dès qu'un nouveau mot est rencontré,
    on le rajoute à notre liste dynamique avec son nombre d'itérations. 
    On trie ensuite la liste de manière décroissante afin que les mots soient
    rangés dans l'ordre décroissant. 
    Complexité : O(n²) pour le rangement des mots (ajout de n mots + n vérifications qu'il n'est pas dans la liste), 
                 O(n log(n)) pour le tri de la liste
*/

typedef struct {
    int cumul_alloc; // liste dynamique de chaînes
    int  cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    int max_alloc;
} InfoMem;

typedef struct {
    int nb_iterations;
    char chaine[1];
    Mot* suivant;
    Mot* prev;
} Mot, *Liste;



int delimite_chaine(char* res, FILE* fichier, char char_debut) {

    res[0] = char_debut;
    int index = 1;

    char c = fgetc(fichier);

    while (!strchr(nonwords, c) && c != EOF) { // tant que c est valide et != fin
        res[index] = c;
        index++;
        c = fgetc(fichier);
    }
    res[index] = '\0'; // ajout du caractère de fin de chaîne
    return 1;
}

int is_a_nonword(char c) {

    if (strchr(nonwords, c))
        return 1;
    return 0;

}

Mot* creer_mot(const char* chaine) {
    // Calculer la taille nécessaire pour la structure et la chaîne
    size_t size = sizeof(Mot) + strlen(chaine); // pas besoin de +1 car chaine[1] est déjà compte
    Mot* mot = (Mot*) malloc(size);
    if (!mot) {
        return NULL;
    }
    mot->nb_iterations = 1;
    strcpy(mot->chaine, chaine);
    mot->suivant = NULL;
    return mot;
}


int ajoute_mot(Liste liste, Mot mot) { // utiliser un pointeur sur liste pour accéder au dernier élément et raccorder le nouvel élém

    for (liste; liste->suivant != NULL; liste = liste->suivant) {

    }
}

int compte_iter_algo1(FILE* fichier){

    char c;
    printf("dans algo");

    while ((c = fgetc(fichier)) != EOF) {
        printf("dans boucle");
        if (!is_a_nonword(c)) { // est un caractère valide

            char chaine[50];
            delimite_chaine(chaine, fichier, c);

            Mot mot;
            if (alloue_mot(&mot, chaine))
                printf(" mot : chaine = %s, iter = %d\n", mot.chaine, mot.nb_iterations);
            else 
                printf("erreur alloue_mot");
            
        }



    }

    return 1;
}

int main(int argc, char* argv[]) {

    Mot* liste_iter = (Mot *) malloc(1000 * sizeof(Mot)); // On commence par ajouter 1000 emplacements de mots dans la liste 

    FILE *file = fopen(argv[1], "r");
        //printf("Il y a %d mots dans le fichier %s\n", compteMots_algo1(file, liste_iter), argv[1]);

    if (!file) 
        return 1;

    compte_iter_algo1(file);
    
    fclose(file);


    return 0;
}
t des mots, 
                 O(n log(n)) pour le tri de la liste
*/

typedef struct {
    int* cumul_alloc; // liste dynamique de chaînes
    int*  cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    int* max_alloc;
} InfoMem;

typedef struct {
    int nb_iterations;
    char chaine[];
} Mot;

int delimite_mot(char* res, FILE* fichier, char char_debut){

    res[0] = char_debut;
    int index = 1;

    char c =  fgetc(fichier);

    while(!strchr(nonwords, c)) {
        res[index] = c;
        index++;
    }

    return 1;
}   

int is_a_nonword(char c) {

    if (strchr(nonwords, c))
        return 1;
    return 0;

}

int alloue_mot(Mot* mot, char* chaine) {

    Mot* cell = (Mot*) malloc(sizeof(Mot));
    if (!cell)
        return 0;

    mot->nb_iterations = 1;
    strcpy(mot->chaine, chaine);
    return 1;
}


int compte_iter_algo1(FILE* fichier){

    char c;

    while ((c = fgetc(fichier)) != EOF) {

        if (!is_a_nonword(c)) { // est un caractère valide

            char chaine[50];
            delimite_mot(chaine, fichier, c);

            Mot mot;
            alloue_mot(&mot, chaine);
            printf(" mot : chaine = %s, iter = %d\n", mot.chaine, mot.nb_iterations);

            
        }



    }

    return 1;
}

int main(int argc, char* argv[]) {

    Mot* liste_iter = (Mot *) malloc(1000 * sizeof(Mot)); // On commence par ajouter 1000 emplacements de mots dans la liste 

    FILE *file = fopen(argv[1], "r");
        //printf("Il y a %d mots dans le fichier %s\n", compteMots_algo1(file, liste_iter), argv[1]);

    if (!file) 
        return 1;

    compte_iter_algo1(file);
    
    fclose(file);


    return 0;
}
