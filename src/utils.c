#include "utils.h"

int is_sorted(int* arr, int n)
{
    /* Un tableau vide ou d'un seul élément est trivialement trié. */
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;   /* deux voisins dans le mauvais ordre : pas trié */
        }
    }
    return 1;           /* aucun problème trouvé : trié */
}

void copy_array(int* src, int* dest, int n)
{
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}
