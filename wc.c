#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
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

    Complexité totale : o(n²))

*/

typedef struct {
    size_t cumul_alloc;
    size_t  cumul_desalloc; 
    size_t max_alloc;
    size_t cumul_taille_mots;
} InfoMem;

typedef struct mot {
    int nb_iterations;
    char* chaine;
    int lenchaine;
} Mot, *Liste;

void *myMalloc(size_t size, InfoMem *infoMem) {
    void * ptr = malloc(size);
    if (ptr) {
        infoMem->cumul_alloc += size;

        size_t mem_courante = infoMem->cumul_alloc - infoMem->cumul_desalloc;
        if (mem_courante > infoMem->max_alloc)
            infoMem->max_alloc = mem_courante;

        return ptr;
    }
    return NULL;
}

void *myRealloc(void *ptr, size_t new_size, InfoMem *infoMem, size_t old_size) {
    if (!new_size) {
        free(ptr);
        infoMem->cumul_desalloc += old_size;
        return NULL;
    }
    void * new_ptr = realloc(ptr, new_size);
    if (new_ptr){
        if (new_ptr != ptr) { // déplacement mémoire
            infoMem->cumul_alloc += new_size;
            infoMem->cumul_desalloc += old_size;
        }
        else {
            if (old_size > new_size) infoMem->cumul_desalloc += (old_size - new_size); // reduction mémoire
            if (new_size > old_size) infoMem->cumul_alloc += (new_size - old_size); //  aggrandissement mémoire
        }

        size_t mem_courante = infoMem->cumul_alloc - infoMem->cumul_desalloc;
        if (mem_courante > infoMem->max_alloc)
            infoMem->max_alloc = mem_courante;

        return new_ptr;
    }
    return ptr;
}

void myFree(void *ptr, InfoMem* infoMem, size_t old_size) {
    if (ptr) {
        free(ptr);
        infoMem->cumul_desalloc += old_size;

        size_t mem_courante = infoMem->cumul_alloc - infoMem->cumul_desalloc;
        if (mem_courante > infoMem->max_alloc)
            infoMem->max_alloc = mem_courante;
    }
}

/*Renvoie 1 si 
le caractère est un 
séparateur, 0 sinon */
int is_a_nonword(char c) {

    if (strchr(nonwords, c))
        return 1;
    return 0;

}

/*Renvoie une copie de la chaîne passée en paramètre;
On choisit de faire une copie et de pas pointer directement
sur la chaîne car cela pose des problèmes quand on passe 
au mot suivant.*/
char* copie_chaine(const char*source, int len, InfoMem* infoMem) {
    char* dest = myMalloc(len + 1, infoMem);
    if (dest == NULL) return NULL;
    for (int i = 0; i < len; i++) {
        dest[i] = source[i];
    }
    dest[len] = '\0';
    return dest;
}

/*Renvoie la suite de caractères
commençant au premier caractère (char_debut)
et finissant avant le prochain séparateur.*/
char* delimite_chaine(FILE *fichier, char char_debut, int* length, InfoMem* infoMem) {

    if (char_debut == EOF)
        return NULL; 

    int capacite = 16; //taille initiale de notre mot
    int len = 0;

    char *res = (char*) myMalloc(capacite * sizeof(char), infoMem);

    if (!res) 
        return NULL; // erreur d'alloc

    res[len++] = char_debut; // on ajoute le premier charactère et on incrémente

    int c = fgetc(fichier); // il faut que c soit un entier car sinon il ne détecte pas EOF qui vaut -1

    while (c != EOF && !is_a_nonword(c)) { // si on inverse le sens des comparaisons, on risque de faire strchr(nw, EOF) (eof = -1) -> impossible

        // on double la capacité si nécessaire
        if (len + 1 >= capacite) {      // +1 pour '\0'
            capacite *= 2;

            char *tmp = (char*) myRealloc(res, capacite, infoMem, capacite/2);
            if (!tmp) {
                myFree(res, infoMem, capacite/2);
                return NULL;
            }

            res = tmp;
        }

        //on ajoute le caractère à la chaîne
        res[len++] = (char) c;
        c = fgetc(fichier);
    }

    if (len == capacite) { // on s'assure qu'il y a de la place pour le '\0'

        char *tmp = (char*) myRealloc(res, capacite + 1, infoMem, capacite);
        if (!tmp) {
            myFree(res, infoMem, capacite);
            return NULL;
        }
        res = tmp;
    }

    res[len] = '\0'; // on ajoute le caractère de fin 
    *length = len;   // on met à jour la longueur
    return res;
}

/* Alloue un Mot et initialise ses champs
   avec la chaîne passée en paramètre */
Mot* alloue_mot(char* chaine, int chaine_len, InfoMem* infoMem) {

    printf("taille de la chaine dans alloue_mot: %d\n", chaine_len);
    if (!chaine) 
        return NULL;

    // calcule la taille nécessaire pour la structure et la chaîne
    size_t size = sizeof(Mot);

    Mot* mot = (Mot*) myMalloc(size, infoMem);
    printf("alloc faite\n");

    // vérification que le pointeur est valide
    if (!mot) {
        myFree(chaine, infoMem, chaine_len);
        return NULL; //l'allocation échoue
    }
    printf("pointeur non-vide\n");
    
    // initialisation des champs du Mot
    mot->nb_iterations = 1;
    printf("nb_iterations initialisé\n");
    mot->chaine = copie_chaine(chaine, chaine_len, infoMem);
    printf("chaine copiée\n");

    // vérification de l'allocation de la chaîne
    if (!mot->chaine) {
        myFree(mot, infoMem, size);
        myFree(chaine, infoMem, chaine_len);
        return NULL;
    }

    //attribution du champs restant
    mot->lenchaine = chaine_len;
    printf("champs recopies\n");
    return mot;
}

/* Cherche le mot dans la liste de mot déjà 
   présents. Si le mot est trouvé, on renvoie 
   son adresse; Sinon, on renvoie NULL */
Mot* cherche_iter(Liste lst, const char* chaine, int lenliste) {

    if (!lst || !chaine) return NULL; // liste vide ou chaîne vide

    printf("\nDEBUT CHERCHE ITER\n\n");
    for (int i=0; i < lenliste; i++) {
        printf("%d < lenliste=%d\n", i, lenliste);

        printf("comparaison avec mot à l'indice %d : %s\n", i, lst[i].chaine);
        if (!strcmp(chaine, lst[i].chaine)) { // les chaînes sont égales
            return &(lst[i]);
        }
        printf("mot non trouvé à l'indice %d\n", i);

    }
    printf("FIN CHERCHE ITER : MOT NON TROUVE\n\n");

    return NULL;
}

/* Affiche les informations d'un
   Mot */
void printMot(Mot mot) {
    printf("mot = %s, de longeur %d itérés %d fois \n", mot.chaine, mot.lenchaine, mot.nb_iterations);
}

/* Affiche les éléments d'une Liste
   ainsi que leurs champs */
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

/* Affiche les informations de performance */
void printInfoMem(InfoMem* im) {
    printf("---- Info Mem ----\n");
    printf("Cumul allocation : %zu octets\n", im->cumul_alloc);
    printf("Cumul désallocation : %zu octets\n", im->cumul_desalloc);
    printf("Max allocation simultanée : %zu octets\n", im->max_alloc);
    printf("------------------\n");
}

/* Incrémente le nombre d'itérations
   d'un Mot */
void incremente_iter(Mot* mot) {
    mot->nb_iterations++;
}

/*Vérifie que la Liste ait assez
   de place pour ajouter un nouveau mot;
   le fait si possible et renvoie 1, et 0 sinon.*/
int ajoute_mot(Liste* lst, Mot mot, int* lenliste, int* capacite, InfoMem* infoMem) { 


    // UTILISER MYREALLOC POUR CECI 
    if (*lenliste >= *capacite) { // reallocation
        printf("NECESSITE DE REALLOUER\n");
        *capacite *= 2;
        
        Liste tmp = (Liste) myRealloc(*lst, (*capacite) * sizeof(Mot), infoMem, (*capacite/2) * sizeof(Mot));

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

/* Change toutes les lettres
d'une chaîne en minuscules */
void chaineLower(char * chaine) {
    for (int i=0; i < strlen(chaine);  i++) {
        chaine[i] = tolower(chaine[i]);
    }
}

/*fonction de comparaison utilisée
 par qsort; La fonction trie en fonction
 du nombre d'itérations*/
int compareMot(const void *a, const void *b) {
    const Mot *m1 = (Liste) a;
    const Mot *m2 = (Liste) b;
    return m2->nb_iterations - m1->nb_iterations; // trie dans l'ordre décroissant
}

/* Trie un tableau de Mot selon 
   le nombre d'itérations*/
void tri_qsort(Liste array_mots, int lenliste){
    printf(" array complete\n");
    qsort(array_mots, lenliste, sizeof(Mot), compareMot);
    printf(" tri array complete\n");
}

/* Initialisation de la structure 
   d'analyse de performance */
void init_infoMem(InfoMem* im) {
    im->cumul_alloc = 0;
    im->cumul_desalloc = 0;
    im->max_alloc = 0;
    im->cumul_taille_mots = 0;
}

/*Premier algorithme de WC*/
int compte_iter_algo1(FILE* fichier, int limite_mots, char* nomfichier){
    //printf("debut algo\n");
    
    // initialisation de la structure d'analyse de performance 
    InfoMem infoMem; 
    init_infoMem(&infoMem);
    printInfoMem(&infoMem);

    // initialisation de la liste de mots
    int capacite = 1000;
    Liste liste_iter = myMalloc(sizeof(Mot) * capacite, &infoMem);
    if (!liste_iter) {
        printf("allocation liste de mots ratée\n");
        return 0; // problème d'allocation
    }

    int nb_total_mots_differents = 0; // mettre ce champ dans InfoMem et update à chaque ajout ? 

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
            Mot* mot = alloue_mot(chaine, len, &infoMem);                // on crée le Mot
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
                else {
                    printf("PROBLEME D'AJOUT DU MOT");
                    myFree(mot, &infoMem, sizeof(Mot));
                }
            }
        }
        myFree(chaine, &infoMem, len); // on libère la chaîne temporaire 
    }

    printf(" tout le texte a été parcouru\n");
    tri_qsort(liste_iter, nb_total_mots_differents);
    printList(liste_iter, nb_total_mots_differents);

    //ecriture_resultat_algo1(liste_iter, nb_total_mots_differents, limite_mots);

    for (int i = 0; i < nb_total_mots_differents; i++) {
        myFree( (void*) liste_iter[i].chaine, &infoMem, strlen(liste_iter[i].chaine));
    }
    myFree(liste_iter, &infoMem, sizeof(Mot) * capacite);

    //printf("sizeof(Mot) = %lu\n", sizeof(Mot));
    //printf("sizeof(InfoMem) = %lu\n", sizeof(InfoMem));

    printInfoMem(&infoMem);
    return 1;
}

void gere_options_compilation(const char* option) {
    if (strcmp(option, "--help") == 0) {
        printf("Pour lancer le programme :");
        printf(" Une fois la compilation faite avec la commande");
        printf("gc");
    } else {
        printf("Option inconnue : %s\n", option);
    }
}

int ecriture_resultat__fichier_algo1(Liste liste_iter, int lenliste, int limite_mots, char* nomfichier) {

    printf("ECRITURE RESULTATS ALGO 1\n");
    int limite = (lenliste < limite_mots) ? lenliste : limite_mots;

    FILE* file = fopen(nomfichier, "w");
    if (!file) {
        perror("ouverture fichier de sortie ratée\n");
        return 0;
    }

    fprintf(file, "-------------------------------\n");
    fprintf(file, "Résultats de l'algorithme 1 :\n");
    fprintf(file, "Affichage des %d mots les plus itérés :\n", limite);
    fprintf(file, "-------------------------------\n");

    for (int i = 0; i < limite; i++) {
        fprintf(file, "%s : %d\n", liste_iter[i].chaine, liste_iter[i].nb_iterations);
    }

    return 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
    fprintf(stderr, "Il y a besoin d'au moins un argument (nom du fichier à lire).\n");
    return 1;
    }

    // initialisation des variables pour utiliser getopt
    int opt;
    int n = 0; // par défaut, affiche les 100 mots les plus usités
    int algo = 1;
    char *sortie = NULL;
    char *perf = NULL;

    /*getopt est une fonction de la bibliothèque standard
    permettant de gérer les options de compilation. 
    la chaîne "n:a:s:l:h" indique que les différentes options 
    sont a, n, s, l, h, et que les 4 premières prennent des arguments*/
    extern char* optarg;
    extern int optind;
    while ((opt = getopt(argc, argv, "n:a:s:l:h")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'a':
                algo = atoi(optarg);
                break;
            case 's':
                sortie = optarg;
                break;
            case 'l':
                perf = optarg;
                break;
            case 'h':
                printf("Usage: %s [-n int] [-a algo1|algo2|algo3] [--help] [-s fichierdesortie] [-l fichierdeperf] fichiers_de_données\n", argv[0]);
                printf(" -n : limite de mots à afficher en commençant par les plus itérés (défaut : pas d'affichage)\n");
                printf(" -a : choix de l'algorithme à utiliser (algo1, algo2, algo3); seulement indiquer le numéro de l'algo désiré.\n");
                printf(" -s : indique si on veut un fichier affichant le résultat ou pas pour le texte donné ou pas. Si oui, mettre en argument un nom de fichier dans lequel écrire\n");
                printf(" -l : nom de fichiers pour l'écriture des performances afin d'en faire des graphiques avec pyplot\n");
                return 0;
            default:
                fprintf(stderr, "Option invalide : %c\n", opt);
                return 1;
        }
    }

    // Après avoir traité les options, optind est l'indice du premier argument non-optionnel
    if (optind >= argc) {
        fprintf(stderr, "Il y a besoin d'un nom de fichier à lire.\n");
        return 1;
    }

    // Utilise argv[optind] pour accéder au nom du fichier
    FILE *file = fopen(argv[optind], "r");
    printf("fichier ouvert f = %s, file = %p\n", argv[optind], file);
    if (!file) {
        perror("ouverture fichier ratée");
        return 1;
    }

    switch (algo) {
        case 1:
            printf("Algorithme 1 sélectionné\n");
            printf("NOM FICHIER SORTIE : %s\n", sortie);
            compte_iter_algo1(file, n, sortie);
            break;
        case 2:
            printf("Algorithme 2 sélectionné\n");
            // compte_iter_algo2(file);
            break;
        case 3:
            printf("Algorithme 3 sélectionné\n");
            // compte_iter_algo3(file);
            break;
        default:
            printf("Algorithme inconnu : %d\n", algo);
            return 1;
    }

    fclose(file);

    return 0;
}
