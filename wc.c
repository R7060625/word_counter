#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define nonwords " ,.;:!?_*\\\n\t0123456789\"\'#][&"

/*PREMIER ALGORITHME : 
    rangement itératif des mots; Dès qu'un nouveau mot est rencontré,
    on le rajoute à notre liste dynamique avec son nombre d'itérations. 
    On trie ensuite la liste de manière décroissante afin que les mots soient
    rangés dans l'ordre décroissant. 
    Complexité : O(n²) pour le rangement des mots (ajout de n mots + n vérifications qu'il n'est pas dans la liste), 
                 O(n log(n)) pour le tri de la liste
                 O(taille_chaine) pour delimite_chaine, alloue_mot
                 O(len(liste)) pour cherche_iter, printList,
                O(1) pour incremente_iter, ajoute_mot

*/

typedef struct {
    size_t cumul_alloc;
    size_t  cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    size_t max_alloc;
    size_t cumul_taille_mots;
} InfoMem;

typedef struct mot {
    int nb_iterations;
    char* chaine;
    int lenchaine;
} Mot, *Liste;

int is_a_nonword(char c) {

    if (strchr(nonwords, c))
        return 1;
    return 0;

}

char* delimite_chaine(FILE *fichier, char char_debut, int* length, InfoMem* infoMem) {

    if (char_debut == EOF)
        return NULL; 

    int capacite = 16; //taille initiale de notre mot
    int len = 0;

    char *res = malloc(capacite * sizeof(char));

    if (!res) 
        return NULL; // erreur d'alloc

    res[len++] = char_debut; // on ajoute le premier charactère

    int c = fgetc(fichier); // il faut que c soit un entier car sinon il ne détecte pas EOF qui vaut -1

    while (c != EOF && !is_a_nonword(c)) { // si on inverse le sens des comparaisons, on risque de faire strchr(nw, EOF) (eof = -1) -> impossible

        // on double la capacité si nécessaire
        if (len + 1 >= capacite) {      // +1 pour '\0'
            //REMPLACER PAR MYREALLOC
            capacite *= 2;
            char *tmp = realloc(res, capacite);
            if (!tmp) {
                free(res);
                return NULL;
            }
            res = tmp;
        }

        //on ajoute le caractère à la chaîne
        res[len++] = (char) c;
        c = fgetc(fichier);
    }

    if (len == capacite) { // on s'assure qu'il y a de la place pour le '\0'

    // REMPLACER PAR MYREALLOC
        char *tmp = realloc(res, capacite + 1);
        if (!tmp) {
            free(res);
            return NULL;
        }
        res = tmp;
    }

    // on ajoute le caractère de fin 
    res[len] = '\0';
    *length = len;
    return res;
}

Mot* alloue_mot(char* chaine, int chaine_len) {

    if (!chaine) 
        return NULL;

    // calcule la taille nécessaire pour la structure et la chaîne
    size_t size = sizeof(Mot);

    Mot* mot = (Mot*) malloc(size);
    printf("alloc faite\n");

    if (!mot) {
        free( (void*) chaine);
        return NULL; //l'allocation échoue
    }
    printf("pointeur non-vide\n");
    
    // initialisation des champs du Mot
    mot->nb_iterations = 1;
    printf("nb_iterations initialisé\n");
    /*char* tmp = strdup(chaine); // on recopie les données de la chaîne et pas juste le pointeur qui est susceptible de changer
    printf("strdup faite: pointeur = %p, contenant %s\n", tmp, tmp);

    if (!tmp) {
        free(mot);
        //free( (void*) chaine);
        return NULL;
    }*/
    mot->chaine = strdup(chaine);  // strdup fait une copie
    if (!mot->chaine) {
        free(mot);
        free( (void*) chaine);
        return NULL;
    }

    mot->lenchaine = chaine_len;
    printf("champs recopies\n");
    return mot;
}

/* Cherche le mot dans la liste de mot déjà 
   présents. Si le mot est trouvé, on renvoie 
   son adresse; Sinon, on renvoie NULL */
Mot* cherche_iter(Liste lst, const char* chaine, int lenliste) {

    if (!lst || !chaine) return NULL; // liste vide

    printf("DEBUT CHERCHE ITER\n");
    for (int i=0; i < lenliste; i++) {
        printf("%d < lenliste=%d\n", i, lenliste);
        if (lst[i].chaine == NULL) {
            printf("Pointeur NULL trouvé à l'indice %d\n", i);
            continue;
        }

        printf("comparaison avec mot à l'indice %d : %s\n", i, lst[i].chaine);
        if (!strcmp(chaine, lst[i].chaine)) { // les chaînes sont égales
            return &(lst[i]);
        }
        printf("mot non trouvé à l'indice %d\n", i);

    }
    printf("FIN CHERCHE ITER : MOT NON TROUVE\n");

    return NULL;
}

void printMot(Mot mot) {
    printf("mot = %s, de longeur %d itérés %d fois \n", mot.chaine, mot.lenchaine, mot.nb_iterations);
}

void printList(Liste lst, int lenliste) {
    printf(" \nLISTE \n");
    printf("%d mots dans la liste\n", lenliste);
    for (int i = 0; i < lenliste; i++) {
        if (lst[i].chaine) {
            printf("mot = %s, longueur %d, itérations %d\n",
                   lst[i].chaine, lst[i].lenchaine, lst[i].nb_iterations);
        } else {
            printf("mot NULL à l'indice %d\n", i);
        }
    }
    printf("FIN LISTE\n\n");
}

void incremente_iter(Mot* mot) {
    mot->nb_iterations++;
}

/* Utilise un pointeur sur liste 
   pour accéder au dernier élément
    et raccorde le nouvel élém*/
int ajoute_mot(Liste* lst, Mot mot, int* lenliste, int* capacite, InfoMem* infoMem) { 


    // UTILISER MYREALLOC POUR CECI 
    if (*lenliste >= *capacite) { // reallocation
        printf("NECESSITE DE REALLOUER\n");
        *capacite *= 2;
        
        Liste tmp = (Liste) realloc(*lst, (*capacite) * sizeof(Mot));

        if (!tmp) {
            printf("NOUVEAU POINTEUR VERS REALLOC INVALIDE\n");
            return 0;
        }
        else 
            *lst = tmp;
        
        printf("POINTEUR CORRECTEMENT REMPLACE\n");
        printList(*lst, *lenliste);
    }
    printf("%d mots dans la liste avant ajout\n", *lenliste);
    printf("PREPARATION A L'AJOUT DANS LA LISTE\n");
    (*lst)[*lenliste] = mot; // ajout du nouveau mot dans la case suivante
    printf("AJOUT CORRECTEMENT FAIT :");
    printMot(mot);
    printf("\n");
    (*lenliste)++;

    return 1;
}

void chaineLower(char * chaine) {
    for (int i=0; i < strlen(chaine);  i++) {
        chaine[i] = tolower(chaine[i]);
    }
}

int compareMot(const void *a, const void *b) {
    const Mot *m1 = (Liste) a;
    const Mot *m2 = (Liste) b;
    return m2->nb_iterations - m1->nb_iterations; // trie dans l'ordre décroissant
}

void tri_qsort(Liste array_mots, int lenliste){
    printf(" array complete\n");
    qsort(array_mots, lenliste, sizeof(Mot), compareMot);
    printf(" tri array complete\n");
}

void init_infoMem(InfoMem* im) {
    im->cumul_alloc = 0;
    im->cumul_desalloc = 0;
    im->max_alloc = 0;
    im->cumul_taille_mots = 0;
}

int compte_iter_algo1(FILE* fichier){
    //printf("debut algo\n");
    
    // initialisation de la liste de mots
    int capacite = 1000;
    Liste liste_iter = malloc(sizeof(Mot) * capacite);
    if (!liste_iter) {
        printf("allocation liste de mots ratée\n");
        return 0; // problème d'allocation
    }

    int nb_total_mots_differents = 0; // mettre ce champ dans InfoMem et update à chaque ajout ? 

    // initialisation de la structure d'analyse de performance 
    InfoMem infoMem; 
    init_infoMem(&infoMem);

    int c;
    while ((c = fgetc(fichier)) != EOF){
        printf("caractere analyse : %c\n", c);
        
        if ( c != EOF && is_a_nonword(c)) {                  // si 'c' est un séparateur,
            c = fgetc(fichier);                     // on récupère un nouveau caractère
            while ( c != EOF && is_a_nonword(c)){            // puis tant qu'il y a des séparateurs,
                c = fgetc(fichier);                 // on prend un nouveau caractère
                printf(" caractere analyse pendant la selection des separateurs :%c", c);
            }
        }

        if (c == EOF) // pour que ça n'analyse pas l'EOF 
            break;

        //printf("debut chaine\n");
        int len=0;
        char* chaine = delimite_chaine(fichier, c, &len, &infoMem); // découpe la suite de lettres entre des séparateurs (aka un mot)
        if (!chaine) {
            printf("problème d'allocation de la chaîne\n");
            return 0; // problème d'allocation
        }

        printf(" chaine = %s\n", chaine);
        chaineLower(chaine);                 // formatage des chaînes pour pas que "Bonjour" et "bonjour" soient des mots différents

        printf("chaine correctement formatée : %s\n", chaine);
        printf("nombre de mots avant recherche : %d\n", nb_total_mots_differents);
        Mot* ptr_mot = cherche_iter(liste_iter, chaine, nb_total_mots_differents);
        printf("ptr_mot = %p\n", ptr_mot);

        if (ptr_mot) {                                   // s'il y a une itération,
            incremente_iter(ptr_mot);                     // on incrémente la valeur
            printf(" correctement incrémenté; nouvelle valeur = %d\n", ptr_mot->nb_iterations);
        } 
        
        else {                                            // sinon,
            printf("on cree le mot\n");
            Mot* mot = alloue_mot(chaine, len);                // on crée le Mot
            printf(" mot : chaine = %s, nb_iter = %d\n", mot->chaine, mot->nb_iterations);

            if (!mot) // problème d'allocation            
                return 0;
            else{
                printf("nb_total_mots_differents = %d\n", nb_total_mots_differents);
                printf("on ajoute le mot\n");

                if (ajoute_mot(&liste_iter, *mot, &nb_total_mots_differents, &capacite, &infoMem)){ // et on l'ajoute à la liste 
                    printf("ajout correctement fait\n");
                    printList(liste_iter, nb_total_mots_differents);
                }
                else 
                    printf("PROBLEME D'AJOUT DU MOT");
                    //free(mot->chaine);
                    free(mot);
                    
            }
        }
        free( (void*) chaine); // on libère la chaîne temporaire 
    }
    printf(" tout le texte a été parcouru\n");
    tri_qsort(liste_iter, nb_total_mots_differents);
    printList(liste_iter, nb_total_mots_differents);

    for (int i = 0; i < nb_total_mots_differents; i++) {
        free( (void*) liste_iter[i].chaine);
    }
    free(liste_iter);
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
    //printf("fichier pas encore ouvert\n");
    FILE *file = fopen(argv[1], "r");
    printf("fichier ouvert f = %s, file = %p\n", argv[1], file);
    if (!file) {
        perror("ouverture fichier ratée\n");
        return 1;
    }

    //printf("debut algo dehors\n");
    compte_iter_algo1(file);
    fclose(file);

    
    return 0;
}

