#ifndef DATA_GEN_H
#define DATA_GEN_H

/* Fixe la graine du generateur pseudo-aleatoire (reproductibilite). */
void data_gen_seed(unsigned long long seed);

/* Chaque fonction alloue (malloc) un tableau de n entiers et retourne
 * son adresse, ou NULL si n <= 0 ou si l'allocation echoue.
 * L'appelant DOIT liberer le tableau avec free(). */
int* generate_random_array(int n);   /* valeurs aleatoires dans [0, 2^31 - 1] */
int* generate_sorted_array(int n);   /* 0, 1, 2, ..., n-1                      */
int* generate_reverse_array(int n);  /* n-1, n-2, ..., 0                       */

#endif
