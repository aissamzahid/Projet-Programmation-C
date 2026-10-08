#ifndef SORTS_H
#define SORTS_H

/* Tous les tris sont en place, croissants, et prennent (tableau, taille). */
void selection_sort(int* arr, int n);  /* O(n^2) toujours               */
void bubble_sort(int* arr, int n);     /* O(n^2), O(n) si deja trie     */
void insertion_sort(int* arr, int n);  /* O(n^2), O(n) si deja trie     */
void quick_sort(int* arr, int n);      /* O(n log n) moyen              */
void merge_sort(int* arr, int n);      /* O(n log n) garanti, memoire O(n) */

#endif
