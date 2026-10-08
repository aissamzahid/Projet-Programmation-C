/* ================================================================
 *  main.c — Programme principal du benchmark des algorithmes de tri
 *  ================================================================
 *  Parcourt les tailles N = 10 000 → 10 000 000 (pas de 50 000),
 *  genere 3 scenarios (aleatoire, trie, inverse), chronometre
 *  chaque algorithme et ecrit les resultats dans data/results.csv.
 * ================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define N_START        10000
#define N_END       10000000
#define N_STEP         50000
#define N_QUAD_MAX    100000   /* seuil pour les tris O(n^2) */

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
    /* ---- 1. Saisie de la configuration du PC ---- */
    char pc_config[256];
    printf("=== Benchmark des algorithmes de tri ===\n\n");
    printf("Entrez la configuration de votre PC\n");
    printf("  (ex: i7-12700H, 16 Go RAM, Windows 11) : ");
    fflush(stdout);

    if (fgets(pc_config, sizeof(pc_config), stdin) == NULL) {
        fprintf(stderr, "Erreur de lecture.\n");
        return 1;
    }
    /* Retirer le retour a la ligne */
    pc_config[strcspn(pc_config, "\r\n")] = '\0';

    /* ---- 2. Initialisation du generateur aleatoire ---- */
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
    fprintf(csv, "Taille,Scenario,Nom_Algo,Temps,PC_Config\n");

    /* ---- 6. Boucle principale du benchmark ---- */
    printf("\nDemarrage du benchmark...\n\n");

    for (int n = N_START; n <= N_END; n += N_STEP) {

        printf("--- N = %d ---\n", n);

        /* 6a. Generation des 3 tableaux source */
        int* sources[NB_SCENARIOS];
        sources[0] = generate_random_array(n);
        sources[1] = generate_sorted_array(n);
        sources[2] = generate_reverse_array(n);

        /* Verification des allocations */
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

        /* 6b. Allocation d'un tableau de travail (copie avant chaque tri) */
        int* work = (int*)malloc((size_t)n * sizeof(int));
        if (work == NULL) {
            fprintf(stderr, "  [ERREUR] Allocation du tableau de travail echouee (N=%d)\n", n);
            for (int s = 0; s < NB_SCENARIOS; s++)
                free(sources[s]);
            continue;
        }

        /* 6c. Pour chaque scenario ... */
        for (int s = 0; s < NB_SCENARIOS; s++) {

            /* ... pour chaque algorithme ... */
            for (int a = 0; a < NB_ALGOS; a++) {

                /* Ignorer les tris quadratiques si N > 100 000 */
                if (algos[a].is_quadratic && n > N_QUAD_MAX) {
                    continue;
                }

                /* Copier le tableau source dans le tableau de travail */
                copy_array(sources[s], work, n);

                /* Mesure du temps */
                double t_start = get_time_sec();
                algos[a].func(work, n);
                double t_end   = get_time_sec();
                double elapsed  = t_end - t_start;

                /* Verification post-tri */
                if (!is_sorted(work, n)) {
                    fprintf(stderr, "  [ECHEC] %s n'a pas trie correctement "
                            "(N=%d, %s)\n", algos[a].name, n, scenario_names[s]);
                }

                /* Ecriture dans le CSV */
                fprintf(csv, "%d,%s,%s,%.9f,%s\n",
                        n, scenario_names[s], algos[a].name, elapsed, pc_config);

                printf("  %-16s | %-10s | %.6f s\n",
                       algos[a].name, scenario_names[s], elapsed);
            }
        }

        /* 6d. Liberation memoire de cette iteration */
        free(work);
        for (int s = 0; s < NB_SCENARIOS; s++)
            free(sources[s]);

        /* Flush du CSV pour ne rien perdre en cas d'arret premature */
        fflush(csv);
    }

    fclose(csv);

    printf("\n=== Benchmark termine ! ===\n");
    printf("Resultats ecrits dans : data/results.csv\n");

    return 0;
}
