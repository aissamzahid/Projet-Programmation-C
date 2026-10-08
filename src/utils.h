#ifndef UTILS_H
#define UTILS_H

/*
 * is_sorted : le "surveillant".
 * Parcourt le tableau et vérifie que chaque élément est <= son voisin de droite.
 * Retourne 1 si le tableau est trié (ordre croissant), 0 sinon.
 */
int is_sorted(int* arr, int n);

/*
 * copy_array : la "photocopieuse".
 * Copie les n éléments de src vers dest.
 * dest doit déjà être alloué (avec malloc) par l'appelant.
 */
void copy_array(int* src, int* dest, int n);

#endif
