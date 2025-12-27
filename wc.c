#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define nonwords " ,.;:!?_*\\\n\t0123456789"

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

typedef struct mot {
    int nb_iterations;
    struct mot* suivant;
    char chaine[];
} Mot, *Liste;

typedef struct {
    char* chaine;
    int size_chaine;
} Chaine;



char *delimite_chaine(FILE *fichier, char char_debut)
{
    int capacite = 16; //taille initiale de notre mot
    int len = 0;

    char *res = malloc(capacite);
    if (!res) return NULL; // erreur d'alloc

    res[len++] = char_debut; // on ajoute le premier charactère

    int c = fgetc(fichier);

    while (c != EOF && !strchr(nonwords, c)) { // si on inverse le sens des comparaisons, on risque de faire strchr(nw, EOF) (eof = -1) -> impossible

        // on double la capacité si nécessaire
        if (len + 1 >= capacite) {      // +1 pour '\0'
            capacite *= 2;
            char *tmp = realloc(res, capacite);
            if (!tmp) {
                free(res);
                return NULL;
            }
            res = tmp;
        }

        //on ajoute le caractère à la chaîne
        res[len++] = (char)c;
        c = fgetc(fichier);
    }

    // on ajoute le caractère de fin 
    res[len] = '\0';
    return res;
}


int is_a_nonword(char c) {

    if (strchr(nonwords, c))
        return 1;
    return 0;

}

Mot* alloue_mot(const char* chaine) {

    // calcule la taille nécessaire pour la structure et la chaîne
    size_t size = sizeof(Mot) + strlen(chaine) + 1; // +1 pour le caractère de fin 

    Mot* mot = (Mot*) malloc(size);

    if (!mot) {
        return NULL; //l'allocation échoue
    }
    
    // initialisation des champs du Mot
    mot->nb_iterations = 1;
    strcpy(mot->chaine, chaine);
    mot->suivant = NULL;
    
    return mot;
}

/* Cherche le mot dans la liste de mot déjà 
   présents. Si le mot est trouvé, on renvoie 
   son adresse; Sinon, on renvoie NULL */
Mot* cherche_iter(Liste lst, char* chaine) {

    if (!lst) return NULL; // liste vide

    while (lst != NULL) {

        if (!strcmp(chaine, (lst)->chaine)) { // les chaînes sont égales
            return lst;
        }

        lst = (lst)->suivant;
    }

    return NULL;
}

void incremente_iter(Mot* mot) {
    mot->nb_iterations++;
}

/* Utilise un pointeur sur liste 
   pour accéder au dernier élément
    et raccorde le nouvel élém*/
int ajoute_mot(Liste* lst, Mot* mot) { 

    while (*lst != NULL) {
        lst = &(*lst)->suivant;
    }
    *lst = mot;
    return 1;
}

/*int addToListEndIn(List *lst, ValueType v) // add an element to the end of
{
    List newCell = createNewCell(v);
    while (*lst != NULL) {
        lst = &(*lst)->next;
    }
    *lst = newCell;
    return 0;
}*/
void printList(Liste lst) {
    printf(" LISTE \n");
    while(lst != NULL){
        printf("mot = %s : itérés %d fois \n", lst->chaine, lst->nb_iterations);
        lst = lst->suivant;
    }
    printf("FIN LISTE\n");
}

/*void printList(List lst, const char msg) {
    while (lst != NULL) {
        printf("%d%c", lst->value, msg);
        lst = lst->next;
    }
    printf("\n");
}*/

void chaineLower(char * chaine) {
    for (int i=0; i < strlen(chaine);  i++) {
        chaine[i] = tolower(chaine[i]);
    }
}

int compte_iter_algo1(FILE* fichier){
    printf("debut algo\n");
    int c;

    Liste liste_iter = NULL;
    printList(liste_iter);
    while ((c = fgetc(fichier)) != EOF){
        printf("caractere analyse : %c\n", c);
        
        if (strchr(nonwords, c)) {                  // si 'c' est un séparateur,
            c = fgetc(fichier);                     // on récupère un nouveau caractère
            while (strchr(nonwords, c)){            // puis tant qu'il y a des séparateurs,
                c = fgetc(fichier);                 // on prend un nouveau caractère
            }
        }

        printf("debut chaine\n");
        char* chaine = delimite_chaine(fichier, c); // découpe la suite de lettres entre des séparateurs (aka un mot)

        printf(" chaine = %s\n", chaine);
        chaineLower(chaine);                 // formatage des chaînes pour pas que "Bonjour" et "bonjour" soient des mots différents

        printf("chaine correctement formatée : %s\n", chaine);
        Mot* ptr_mot = cherche_iter(liste_iter, chaine);
        printf("ptr_mot = %p\n", ptr_mot);

        if ( ptr_mot ) {          // s'il y a une itération,
            incremente_iter(ptr_mot);                     // on incrémente la valeur
            printf(" correctement incrémenté; nouvelle valeur = %d\n", ptr_mot->nb_iterations);
        } 
        else {                                            // sinon,
            Mot* mot = alloue_mot(chaine);                // on crée le Mot
            printf(" mot : chaine = %s, nb_iter = %d, suivant = %p\n", mot->chaine, mot->nb_iterations, mot->suivant);
            if (!mot) // problème d'allocation            
                return 0;
            else{
                printf("on ajoute le mot\n");
                if (ajoute_mot(&liste_iter, mot))         // et on l'ajoute à la liste 
                    printf("ajout correctement fait\n");
                printList(liste_iter);
            }
        }

    }

    return 1;
}

int main(int argc, char* argv[]) {

    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++){                 // liste les arguments
        printf("arg %d = %s\n", i, argv[i]);
    }

    if (argc < 2) {
    fprintf(stderr, "Usage : %s <nom_fichier>\n", argv[0]);
    return 1;
}

    printf("fichier pas encore ouvert\n");
    FILE *file = fopen(argv[1], "r");
    printf("fichier ouvert f = %s, file = %p\n", argv[1], file);
    if (!file) {
        perror("ouverture fichier ratée\n");
        return 1;
    }

    printf("debut algo dehors\n");
    compte_iter_algo1(file);
    
    fclose(file);


    return 0;
}
