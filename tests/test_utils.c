/* Petit test indépendant du module utils (ne dépend pas du code des autres). */
#include <stdio.h>
#include <stdlib.h>
#include "../src/utils.h"

static int echecs = 0;

static void verifier(const char* nom, int condition)
{
    printf("[%s] %s\n", condition ? "OK" : "ECHEC", nom);
    if (!condition) echecs++;
}

int main(void)
{
    int trie[]    = {1, 2, 3, 4, 5};
    int inverse[] = {5, 4, 3, 2, 1};
    int egaux[]   = {7, 7, 7};
    int melange[] = {1, 3, 2, 4, 5};
    int un[]      = {42};

    verifier("is_sorted : tableau trie",            is_sorted(trie, 5) == 1);
    verifier("is_sorted : tableau inverse",         is_sorted(inverse, 5) == 0);
    verifier("is_sorted : valeurs egales",          is_sorted(egaux, 3) == 1);
    verifier("is_sorted : erreur au milieu",        is_sorted(melange, 5) == 0);
    verifier("is_sorted : un seul element",         is_sorted(un, 1) == 1);
    verifier("is_sorted : tableau vide (n = 0)",    is_sorted(trie, 0) == 1);

    int* copie = malloc(5 * sizeof(int));
    if (copie == NULL) { printf("malloc a echoue\n"); return 1; }
    copy_array(inverse, copie, 5);
    int identique = 1;
    for (int i = 0; i < 5; i++) if (copie[i] != inverse[i]) identique = 0;
    verifier("copy_array : copie identique", identique);

    copie[0] = 999;  /* modifier la copie ne doit pas toucher l'original */
    verifier("copy_array : l'original n'est pas modifie", inverse[0] == 5);
    free(copie);

    printf(echecs == 0 ? "\nTous les tests passent.\n" : "\n%d test(s) en echec.\n", echecs);
    return echecs != 0;
}
