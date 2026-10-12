#include <stdio.h>
#include <stdlib.h>
#include "array_io.h"
#include "data_gen.h"

int save_array(const char* path, const int* arr, int n)
{
    if (path == NULL || arr == NULL || n <= 0) return -1;
    FILE* f = fopen(path, "wb");
    if (f == NULL) return -1;

    int ok = (fwrite(&n, sizeof(int), 1, f) == 1) &&
             (fwrite(arr, sizeof(int), (size_t)n, f) == (size_t)n);
    if (fclose(f) != 0) ok = 0;
    return ok ? 0 : -1;
}

int save_array_txt(const char* path, const int* arr, int n)
{
    if (path == NULL || arr == NULL || n <= 0) return -1;
    FILE* f = fopen(path, "w");
    if (f == NULL) return -1;

    int ok = (fprintf(f, "%d\n", n) > 0);
    for (int i = 0; ok && i < n; i++)
        ok = (fprintf(f, "%d\n", arr[i]) > 0);
    if (fclose(f) != 0) ok = 0;
    return ok ? 0 : -1;
}

int* load_array(const char* path, int* n)
{
    if (path == NULL || n == NULL) return NULL;
    FILE* f = fopen(path, "rb");
    if (f == NULL) return NULL;

    int size = 0;
    if (fread(&size, sizeof(int), 1, f) != 1 || size <= 0) {
        fclose(f);
        return NULL;
    }
    int* arr = (int*)malloc((size_t)size * sizeof(int));
    if (arr == NULL) { fclose(f); return NULL; }

    if (fread(arr, sizeof(int), (size_t)size, f) != (size_t)size) {
        free(arr);          /* fichier tronque ou corrompu */
        fclose(f);
        return NULL;
    }
    fclose(f);
    *n = size;
    return arr;
}

int save_dataset(const char* dir, int n)
{
    if (dir == NULL || n <= 0) return -1;

    const char* names[3] = { "random", "sorted", "reverse" };
    int* arrays[3];
    arrays[0] = generate_random_array(n);
    arrays[1] = generate_sorted_array(n);
    arrays[2] = generate_reverse_array(n);

    int status = 0;
    for (int i = 0; i < 3; i++) {
        if (arrays[i] == NULL) { status = -1; continue; }
        char path[512];
        snprintf(path, sizeof(path), "%s/%s_%d.bin", dir, names[i], n);
        if (save_array(path, arrays[i], n) != 0) status = -1;
        free(arrays[i]);
    }
    return status;
}
