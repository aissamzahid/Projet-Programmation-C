/* ================================================================
 *  main.c — Programme principal du benchmark des algorithmes de tri
 *  ================================================================
 *  Parcourt les tailles N = 10 000 → 10 000 000 (pas de 50 000),
 *  genere 3 scenarios (aleatoire, trie, inverse), chronometre
 *  chaque algorithme et ecrit les resultats dans data/results.csv.
 * ================================================================ */

#include <stdio.h>
#include <stdlib.h>

/* ---------- Mesure de temps haute precision ---------- */
#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
#endif

#include "data_gen.h"
#include "sorts.h"
#include "utils.h"

/* ---------- Constantes du benchmark ---------- */
/* Phase 1 : tous les algorithmes (y compris quadratiques) */
#define N_QUAD_START   10000
#define N_QUAD_END     50000
#define N_QUAD_STEP    10000

/* Phase 2 : algorithmes O(n log n) uniquement */
#define N_FAST_START   100000
#define N_FAST_END  10000000
#define N_FAST_STEP   100000

#define NB_ALGOS  5
#define NB_SCENARIOS 3

/* ---------- Descripteur d'algorithme ---------- */
typedef void (*sort_fn)(int*, int);

typedef struct {
    const char* name;
    sort_fn     func;
    int         is_quadratic;   /* 1 = O(n^2), 0 = O(n log n) */
} Algorithm;

/* ---------- Fonction de mesure du temps (secondes) ---------- */
static double get_time_sec(void)
{
#ifdef _WIN32
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#endif
}

/* ---------- Creation du dossier data/ si necessaire ---------- */
static void ensure_data_dir(void)
{
#ifdef _WIN32
    CreateDirectoryA("data", NULL);
#else
    /* Commande POSIX simple ; ignore l'erreur si deja existant */
    system("mkdir -p data");
#endif
}

/* ================================================================
 *  main
 * ================================================================ */
int main(void)
{
    printf("=== Benchmark des algorithmes de tri ===\n\n");
    printf("NOTE : Remplissez data/pc_config.txt avec votre configuration PC\n");
    printf("       (cette info sera utilisee par le script de visualisation).\n\n");

    /* ---- 1. Initialisation du generateur aleatoire ---- */
    data_gen_seed(0);   /* graine automatique (basee sur time) */

    /* ---- 3. Table des algorithmes ---- */
    Algorithm algos[NB_ALGOS] = {
        { "Selection Sort", selection_sort, 1 },
        { "Bubble Sort",    bubble_sort,    1 },
        { "Insertion Sort", insertion_sort, 1 },
        { "Quick Sort",     quick_sort,     0 },
        { "Merge Sort",     merge_sort,     0 }
    };

    /* ---- 4. Noms des scenarios ---- */
    const char* scenario_names[NB_SCENARIOS] = {
        "Aleatoire", "Trie", "Inverse"
    };

    /* ---- 5. Ouverture du fichier CSV ---- */
    ensure_data_dir();

    FILE* csv = fopen("data/results.csv", "w");
    if (csv == NULL) {
        fprintf(stderr, "Impossible de creer data/results.csv\n");
        return 1;
    }
    fprintf(csv, "Taille,Scenario,Nom_Algo,Temps\n");

    /* ---- 6. Boucle principale du benchmark ---- */
    printf("\nDemarrage du benchmark...\n\n");

    /* ======== Phase 1 : tous les algorithmes (10k -> 50k, pas 10k) ======== */
    printf("=== Phase 1 : tous les algorithmes (N = %d a %d, pas %d) ===\n\n",
           N_QUAD_START, N_QUAD_END, N_QUAD_STEP);

    for (int n = N_QUAD_START; n <= N_QUAD_END; n += N_QUAD_STEP) {

        printf("--- N = %d ---\n", n);

        int* sources[NB_SCENARIOS];
        sources[0] = generate_random_array(n);
        sources[1] = generate_sorted_array(n);
        sources[2] = generate_reverse_array(n);

        int alloc_ok = 1;
        for (int s = 0; s < NB_SCENARIOS; s++) {
            if (sources[s] == NULL) {
                fprintf(stderr, "  [ERREUR] Allocation echouee pour N=%d, scenario=%s\n",
                        n, scenario_names[s]);
                alloc_ok = 0;
            }
        }
        if (!alloc_ok) {
            for (int s = 0; s < NB_SCENARIOS; s++)
                free(sources[s]);
            continue;
        }

        int* work = (int*)malloc((size_t)n * sizeof(int));
        if (work == NULL) {
            fprintf(stderr, "  [ERREUR] Allocation du tableau de travail echouee (N=%d)\n", n);
            for (int s = 0; s < NB_SCENARIOS; s++)
                free(sources[s]);
            continue;
        }

        for (int s = 0; s < NB_SCENARIOS; s++) {
            for (int a = 0; a < NB_ALGOS; a++) {
                copy_array(sources[s], work, n);

                double t_start = get_time_sec();
                algos[a].func(work, n);
                double t_end   = get_time_sec();
                double elapsed  = t_end - t_start;

                if (!is_sorted(work, n)) {
                    fprintf(stderr, "  [ECHEC] %s n'a pas trie correctement "
                            "(N=%d, %s)\n", algos[a].name, n, scenario_names[s]);
                }

                fprintf(csv, "%d,%s,%s,%.7f\n",
                        n, scenario_names[s], algos[a].name, elapsed);

                printf("  %-16s | %-10s | %.6f s\n",
                       algos[a].name, scenario_names[s], elapsed);
            }
        }

        free(work);
        for (int s = 0; s < NB_SCENARIOS; s++)
            free(sources[s]);
        fflush(csv);
    }

    /* ======== Phase 2 : algorithmes O(n log n) (110k -> 10M, pas 100k) ======== */
    printf("\n=== Phase 2 : Quick Sort & Merge Sort (N = %d a %d, pas %d) ===\n\n",
           N_FAST_START, N_FAST_END, N_FAST_STEP);

    for (int n = N_FAST_START; n <= N_FAST_END; n += N_FAST_STEP) {

        printf("--- N = %d ---\n", n);

        int* sources[NB_SCENARIOS];
        sources[0] = generate_random_array(n);
        sources[1] = generate_sorted_array(n);
        sources[2] = generate_reverse_array(n);

        int alloc_ok = 1;
        for (int s = 0; s < NB_SCENARIOS; s++) {
            if (sources[s] == NULL) {
                fprintf(stderr, "  [ERREUR] Allocation echouee pour N=%d, scenario=%s\n",
                        n, scenario_names[s]);
                alloc_ok = 0;
            }
        }
        if (!alloc_ok) {
            for (int s = 0; s < NB_SCENARIOS; s++)
                free(sources[s]);
            continue;
        }

        int* work = (int*)malloc((size_t)n * sizeof(int));
        if (work == NULL) {
            fprintf(stderr, "  [ERREUR] Allocation du tableau de travail echouee (N=%d)\n", n);
            for (int s = 0; s < NB_SCENARIOS; s++)
                free(sources[s]);
            continue;
        }

        for (int s = 0; s < NB_SCENARIOS; s++) {
            for (int a = 0; a < NB_ALGOS; a++) {
                /* Phase 2 : ignorer les tris quadratiques */
                if (algos[a].is_quadratic) continue;

                copy_array(sources[s], work, n);

                double t_start = get_time_sec();
                algos[a].func(work, n);
                double t_end   = get_time_sec();
                double elapsed  = t_end - t_start;

                if (!is_sorted(work, n)) {
                    fprintf(stderr, "  [ECHEC] %s n'a pas trie correctement "
                            "(N=%d, %s)\n", algos[a].name, n, scenario_names[s]);
                }

                fprintf(csv, "%d,%s,%s,%.7f\n",
                        n, scenario_names[s], algos[a].name, elapsed);

                printf("  %-16s | %-10s | %.6f s\n",
                       algos[a].name, scenario_names[s], elapsed);
            }
        }

        free(work);
        for (int s = 0; s < NB_SCENARIOS; s++)
            free(sources[s]);
        fflush(csv);
    }

    fclose(csv);

    printf("\n=== Benchmark termine ! ===\n");
    printf("Resultats ecrits dans : data/results.csv\n");

    return 0;
}
