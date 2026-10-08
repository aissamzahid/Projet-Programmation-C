#include <stdlib.h>
#include <time.h>
#include "data_gen.h"

/* Generateur xorshift64 : rapide, et couvre toute la plage des int,
 * contrairement a rand() dont RAND_MAX vaut seulement 32767 sous Windows. */
static unsigned long long state = 0;

void data_gen_seed(unsigned long long seed)
{
    state = seed ? seed : 88172645463325252ULL;  /* l'etat ne doit jamais etre 0 */
}

static unsigned long long next_rand(void)
{
    if (state == 0)
        data_gen_seed((unsigned long long)time(NULL));
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

int* generate_random_array(int n)
{
    if (n <= 0) return NULL;
    int* arr = (int*)malloc((size_t)n * sizeof(int));
    if (arr == NULL) return NULL;
    for (int i = 0; i < n; i++)
        arr[i] = (int)(next_rand() >> 33);   /* 31 bits -> entier positif */
    return arr;
}

int* generate_sorted_array(int n)
{
    if (n <= 0) return NULL;
    int* arr = (int*)malloc((size_t)n * sizeof(int));
    if (arr == NULL) return NULL;
    for (int i = 0; i < n; i++)
        arr[i] = i;
    return arr;
}

int* generate_reverse_array(int n)
{
    if (n <= 0) return NULL;
    int* arr = (int*)malloc((size_t)n * sizeof(int));
    if (arr == NULL) return NULL;
    for (int i = 0; i < n; i++)
        arr[i] = n - 1 - i;
    return arr;
}
