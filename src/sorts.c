#include <stdio.h>
#include <stdlib.h>
#include "sorts.h"

static void swap(int* a, int* b)
{
    int t = *a; *a = *b; *b = t;
}

/* ---------- Tri par selection ---------- */
void selection_sort(int* arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j] < arr[min]) min = j;
        if (min != i) swap(&arr[i], &arr[min]);
    }
}

/* ---------- Tri a bulles (avec arret anticipe) ---------- */
void bubble_sort(int* arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* ---------- Tri par insertion ---------- */
void insertion_sort(int* arr, int n)
{
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* ---------- Tri rapide ---------- */
/* Pivot = mediane de trois ; partition de Hoare ; on recurse sur la plus
 * petite partie et on boucle sur la plus grande => profondeur <= log2(n). */
static void quick_rec(int* a, int lo, int hi)
{
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < a[lo]) swap(&a[mid], &a[lo]);
        if (a[hi]  < a[lo]) swap(&a[hi],  &a[lo]);
        if (a[hi]  < a[mid]) swap(&a[hi], &a[mid]);
        int pivot = a[mid];

        int i = lo, j = hi;
        while (i <= j) {
            while (a[i] < pivot) i++;
            while (a[j] > pivot) j--;
            if (i <= j) { swap(&a[i], &a[j]); i++; j--; }
        }
        /* sous-tableaux : [lo..j] et [i..hi] */
        if (j - lo < hi - i) { quick_rec(a, lo, j); lo = i; }
        else                 { quick_rec(a, i, hi); hi = j; }
    }
}

void quick_sort(int* arr, int n)
{
    if (n > 1) quick_rec(arr, 0, n - 1);
}

/* ---------- Tri par fusion ---------- */
static void merge_rec(int* a, int* tmp, int lo, int hi)
{
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    merge_rec(a, tmp, lo, mid);
    merge_rec(a, tmp, mid + 1, hi);

    for (int k = lo; k <= hi; k++) tmp[k] = a[k];   /* copie de la zone */
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi)
        a[k++] = (tmp[i] <= tmp[j]) ? tmp[i++] : tmp[j++];
    while (i <= mid) a[k++] = tmp[i++];
    while (j <= hi)  a[k++] = tmp[j++];
}

void merge_sort(int* arr, int n)
{
    if (n < 2) return;
    int* tmp = (int*)malloc((size_t)n * sizeof(int));  /* un seul malloc */
    if (tmp == NULL) {
        fprintf(stderr, "merge_sort: allocation impossible\n");
        return;
    }
    merge_rec(arr, tmp, 0, n - 1);
    free(tmp);
}
