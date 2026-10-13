#ifndef ARRAY_IO_H
#define ARRAY_IO_H

/* Sauvegarde / chargement des tableaux generes (module independant).
 *
 * Format binaire (.bin) : un int (n) suivi de n int, tels quels en memoire.
 * Format texte   (.txt) : premiere ligne = n, puis une valeur par ligne.
 *
 * Les fonctions int retournent 0 en cas de succes, -1 en cas d'erreur. */

int  save_array(const char* path, const int* arr, int n);      /* binaire */
int  save_array_txt(const char* path, const int* arr, int n);  /* texte lisible */

/* Charge un fichier .bin : alloue le tableau (malloc), ecrit sa taille dans
 * *n et retourne son adresse, ou NULL en cas d'erreur.
 * L'appelant DOIT liberer le tableau avec free(). */
int* load_array(const char* path, int* n);

/* Genere les 3 scenarios pour la taille n (via data_gen) et les enregistre
 * dans le dossier dir (qui doit exister) :
 *   dir/random_<n>.bin, dir/sorted_<n>.bin, dir/reverse_<n>.bin */
int  save_dataset(const char* dir, int n);

#endif
