# Tâches — Benchmark Algorithmes de Tri

## 1. Structure du Projet
- [ ] Créer l'arborescence des dossiers (`src/`, `data/`, `scripts/`)

## 2. Module Génération de Données (`src/data_gen.h` & `src/data_gen.c`)
- [ ] Écrire `data_gen.h` : prototypes des fonctions de génération
- [ ] Implémenter `generate_random_array(int n)` — allocation dynamique + remplissage aléatoire
- [ ] Implémenter `generate_sorted_array(int n)` — allocation dynamique + remplissage croissant
- [ ] Implémenter `generate_reverse_array(int n)` — allocation dynamique + remplissage décroissant

## 3. Module Algorithmes de Tri (`src/sorts.h` & `src/sorts.c`)
- [ ] Écrire `sorts.h` : prototypes des 5 algorithmes
- [ ] Implémenter `selection_sort(int* arr, int n)`
- [ ] Implémenter `bubble_sort(int* arr, int n)`
- [ ] Implémenter `insertion_sort(int* arr, int n)`
- [ ] Implémenter `quick_sort(int* arr, int n)`
- [ ] Implémenter `merge_sort(int* arr, int n)` (avec allocation dynamique interne pour la fusion)

## 4. Module Utilitaires (`src/utils.h` & `src/utils.c`)
- [ ] Écrire `utils.h` : prototypes des fonctions utilitaires
- [ ] Implémenter `is_sorted(int* arr, int n)` — vérification post-tri
- [ ] Implémenter `copy_array(int* src, int* dest, int n)` — copie de tableau

## 5. Programme Principal (`src/main.c`)
- [ ] Saisie manuelle de la configuration du PC
- [ ] Boucle de benchmark : N = 10 000 → 10 000 000, pas de 50 000
  - [ ] Allocation dynamique des 3 tableaux (Aléatoire, Trié, Inversé) via `data_gen.c`
  - [ ] Vérification de la taille (ignorer les tris quadratiques O(n^2) si N > 100 000)
  - [ ] Mesure du temps de haute précision (`clock()` / `QueryPerformanceCounter`)
  - [ ] Vérification `is_sorted()` après chaque tri
  - [ ] Écriture des résultats dans `data/results.csv`
  - [ ] Libération mémoire (`free()`) à chaque itération

## 6. Makefile
- [ ] Écrire le `Makefile` avec les cibles :
  - [ ] `make` / `make all` — compilation complète
  - [ ] `make clean` — suppression des fichiers objets et de l'exécutable

## 7. Visualisation Python (`scripts/plot.py`)
- [ ] Lire `data/results.csv` avec `pandas`
- [ ] Générer 3 graphiques (Aléatoire, Trié, Inversé) avec `matplotlib`
- [ ] Intégrer la configuration PC (saisie manuelle) dans le titre des graphiques

## 8. Vérification & Tests
- [ ] Compiler le projet (`make`)
- [ ] Tester la correction des algorithmes sur de petits tableaux
- [ ] Lancer un test réduit (N max = 100 000, pas de 10 000) pour valider le CSV et le pipeline
- [ ] Vérifier l'absence de fuites mémoire
- [ ] Lancer le benchmark complet
- [ ] Générer les graphiques finaux avec `plot.py`
