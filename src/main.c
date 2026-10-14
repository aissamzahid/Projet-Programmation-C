/* ================================================================
 *  main.c — Programme principal du benchmark des algorithmes de tri
 *  ================================================================
 *  Parcourt les tailles N = 10 000 → 10 000 000,
 *  charge les 3 scenarios depuis des fichiers .bin persistants
 *  (dossier data_generated/), chronometre chaque algorithme
 *  et ecrit les resultats dans data/results.csv.
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
#include "array_io.h"
#include "sorts.h"
#include "utils.h"

/* ---------- Constantes du benchmark ---------- */
/* Phase 1 : tous les algorithmes (y compris quadratiques) */
#define N_QUAD_START   10000
#define N_QUAD_END     100000
#define N_QUAD_STEP    10000

/* Phase 2 : algorithmes O(n log n) uniquement */
#define N_FAST_START   200000
#define N_FAST_END  10000000
#define N_FAST_STEP   100000

#define NB_ALGOS  5
#define NB_SCENARIOS 3

/* Dossier de stockage des fichiers .bin generes */
#define DATA_GEN_DIR "data_generated"

/* Prefixes des fichiers .bin (meme ordre que scenario_names) */
static const char* FILE_PREFIXES[NB_SCENARIOS] = {
    "random", "sorted", "reverse"
};

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

/* ---------- Lecture de la configuration PC depuis data/pc_config.txt ---------- */
/* Remplace les virgules par des points-virgules pour ne pas corrompre le CSV.   */
static void read_pc_config(char* buf, size_t buf_size)
{
    const char* path = "data/pc_config.txt";
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        /* Fichier absent : valeur par defaut */
        snprintf(buf, buf_size, "Configuration inconnue");
        return;
    }

    /* Lire la premiere ligne non-vide */
    buf[0] = '\0';
    while (fgets(buf, (int)buf_size, f)) {
        /* Supprimer le saut de ligne eventuel */
        size_t len = strlen(buf);
        while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
            buf[--len] = '\0';
        if (len > 0) break;   /* on garde la premiere ligne non-vide */
    }
    fclose(f);

    /* Remplacer les virgules par des points-virgules (separateur CSV reserve) */
    for (size_t i = 0; buf[i]; i++)
        if (buf[i] == ',') buf[i] = ';';

    if (buf[0] == '\0')
        snprintf(buf, buf_size, "Configuration inconnue");
}

/* ---------- Construction du slug de nom de fichier depuis pc_config ---------- */
/* Produit un identifiant fichier sur : espaces -> '_', on garde [A-Za-z0-9._-]. */
static void make_csv_slug(const char* config, char* slug, size_t slug_size)
{
    size_t j = 0;
    for (size_t i = 0; config[i] && j < slug_size - 1; i++) {
        char c = config[i];
        if (c == ' ' || c == '\t')              { slug[j++] = '_'; }
        else if ((c >= 'A' && c <= 'Z') ||
                 (c >= 'a' && c <= 'z') ||
                 (c >= '0' && c <= '9') ||
                 c == '-' || c == '.')           { slug[j++] = c;   }
        /* autres caracteres (; ( ) / ...) ignores */
    }
    /* Eviter un slug vide */
    if (j == 0) {
        const char* fallback = "machine";
        for (size_t k = 0; fallback[k] && j < slug_size - 1; k++)
            slug[j++] = fallback[k];
    }
    slug[j] = '\0';
}

/* ---------- Creation du dossier data_generated/ si necessaire ---------- */
static void ensure_data_generated_dir(void)
{
#ifdef _WIN32
    CreateDirectoryA(DATA_GEN_DIR, NULL);
#else
    system("mkdir -p " DATA_GEN_DIR);
#endif
}

/* ---------- Chargement d'un scenario depuis un fichier .bin ---------- */
/* Tente de charger le fichier ; s'il n'existe pas, genere le dataset   */
/* complet pour la taille n puis recharge le fichier demande.           */
static int* load_scenario(int scenario_idx, int n)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s_%d.bin",
             DATA_GEN_DIR, FILE_PREFIXES[scenario_idx], n);

    int loaded_n = 0;
    int* arr = load_array(path, &loaded_n);

    if (arr == NULL) {
        /* Fichier absent : generer les 3 scenarios sur disque */
        printf("  [INFO] Dataset N=%d absent, generation dans %s/...\n",
               n, DATA_GEN_DIR);
        data_gen_seed(0);
        save_dataset(DATA_GEN_DIR, n);
        arr = load_array(path, &loaded_n);
    }
    return arr;
}

/* ================================================================
 *  main
 * ================================================================ */
int main(void)
{
    printf("=== Benchmark des algorithmes de tri ===\n\n");
    printf("NOTE : Remplissez data/pc_config.txt avec votre configuration PC\n");
    printf("       (cette info sera ecrite dans chaque ligne de data/results.csv).\n\n");

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

    /* ---- 5. Creation des dossiers et ouverture du fichier CSV ---- */
    ensure_data_dir();
    ensure_data_generated_dir();

    /* ---- Lecture de la configuration PC ---- */
    char pc_config[256];
    read_pc_config(pc_config, sizeof(pc_config));
    printf("Configuration PC detectee : %s\n\n", pc_config);

    /* ---- Construction du nom de fichier CSV : data/results_<slug>.csv ---- */
    char csv_slug[200];
    make_csv_slug(pc_config, csv_slug, sizeof(csv_slug));

    char csv_path[512];
    snprintf(csv_path, sizeof(csv_path), "data/results_%s.csv", csv_slug);
    printf("Fichier de resultats : %s\n\n", csv_path);

    FILE* csv = fopen(csv_path, "w");
    if (csv == NULL) {
        fprintf(stderr, "Impossible de creer %s\n", csv_path);
        return 1;
    }
    fprintf(csv, "Taille,Scenario,Nom_Algo,Temps,PC_Config\n");

    /* ---- 6. Boucle principale du benchmark ---- */
    printf("\nDemarrage du benchmark...\n\n");

    /* ======== Phase 1 : tous les algorithmes (10k -> 50k, pas 10k) ======== */
    printf("=== Phase 1 : tous les algorithmes (N = %d a %d, pas %d) ===\n\n",
           N_QUAD_START, N_QUAD_END, N_QUAD_STEP);

    for (int n = N_QUAD_START; n <= N_QUAD_END; n += N_QUAD_STEP) {

        printf("--- N = %d ---\n", n);

        int* sources[NB_SCENARIOS];
        for (int s = 0; s < NB_SCENARIOS; s++)
            sources[s] = load_scenario(s, n);

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

                fprintf(csv, "%d,%s,%s,%.7f,%s\n",
                        n, scenario_names[s], algos[a].name, elapsed, pc_config);

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
        for (int s = 0; s < NB_SCENARIOS; s++)
            sources[s] = load_scenario(s, n);

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

                fprintf(csv, "%d,%s,%s,%.7f,%s\n",
                        n, scenario_names[s], algos[a].name, elapsed, pc_config);

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
    printf("Resultats ecrits dans : %s\n", csv_path);

    return 0;
}
