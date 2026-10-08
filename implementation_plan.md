# Plan d'implémentation : Benchmark des Algorithmes de Tri

Ce document détaille le plan final pour concevoir, implémenter et analyser les performances de différents algorithmes de tri en langage C, avec une visualisation en Python. 

## User Review Required

J'ai détaillé la structure du projet avec le nouveau fichier de génération de données et l'allocation dynamique. Si cette structure vous convient, je passe immédiatement à la création des fichiers du projet.

## Solution optimale : Limiter les algorithmes quadratiques

Pour éviter un temps d'attente excessif et garantir la fiabilité du benchmark, nous allons utiliser une limite stricte basée sur la complexité algorithmique :
- Les algorithmes en $O(n^2)$ (Tri à bulles, Tri par insertion, Tri par sélection) sont limités à une taille de tableau maximale de $N = 100\ 000$. Au-delà de cette taille, ils ne seront plus exécutés pour éviter de bloquer le programme.
- Les algorithmes en $O(n \log n)$ (Tri rapide, Tri par fusion) continueront d'être testés sur toutes les tailles de tableaux.

## Proposed Changes

Le projet sera structuré en deux parties principales : le code C pour la logique et le benchmark, et le script Python pour la visualisation.

### 1. Structure du Projet Détaillée

Le projet sera découpé en plusieurs fichiers distincts pour garantir une bonne modularité :

- **`src/data_gen.h` & `src/data_gen.c` (Fichier indépendant pour les données)**
  - Fonctions pour générer un tableau de taille $N$ rempli de valeurs aléatoires.
  - Fonctions pour générer un tableau déjà trié.
  - Fonctions pour générer un tableau trié en ordre inverse.
  - *Note :* Ces fonctions utiliseront l'allocation dynamique (`malloc`) pour allouer l'espace nécessaire et retourneront un pointeur vers le tableau.

- **`src/sorts.h` & `src/sorts.c` (Algorithmes de tri)**
  - Implémentation du Tri par sélection (Selection Sort).
  - Implémentation du Tri à bulles (Bubble Sort).
  - Implémentation du Tri par insertion (Insertion Sort).
  - Implémentation du Tri rapide (Quick Sort).
  - Implémentation du Tri par fusion (Merge Sort) : *inclura l'allocation dynamique temporaire d'un tableau pour la fusion (avec `malloc` et `free` interne).*

- **`src/utils.h` & `src/utils.c` (Utilitaires)**
  - Fonction `is_sorted(int* arr, int n)` pour vérifier si le tri a réussi.
  - Fonction `copy_array(int* src, int* dest, int n)` pour dupliquer les tableaux générés sans altérer l'original avant chaque test.

- **`src/main.c` (Programme Principal)**
  - Saisie de la configuration du PC.
  - Boucle allant de $N = 10\ 000$ à $10\ 000\ 000$ par pas de $50\ 000$.
  - À chaque itération :
    - Allocation dynamique des tableaux (via `data_gen.c`).
    - Exécution des algorithmes (avec omission des tris quadratiques pour N > 100 000).
    - Mesure précise du temps (`QueryPerformanceCounter` sous Windows ou `clock()` du C).
    - Libération de la mémoire (`free()`) pour éviter les fuites de mémoire.
    - Écriture asynchrone ou séquentielle dans le fichier `results.csv`.

- **`data/results.csv`**
  - Fichier généré contenant les colonnes : `Taille, Scénario, Nom_Algo, Temps, PC_Config`.

- **`scripts/plot.py`**
  - Script Python avec `matplotlib` pour lire le `results.csv` et tracer l'évolution du temps d'exécution (avec 3 graphiques : Aléatoire, Trié, Inversé).

- **`Makefile`**
  - Instructions pour compiler facilement : commande `make` pour construire l'exécutable, et `make clean` pour supprimer les fichiers objets compilés.

## Verification Plan

### Test d'allocation dynamique
- Exécution du code compilé avec un petit sous-ensemble de données tout en surveillant la mémoire RAM pour vérifier qu'il n'y a aucune fuite (`free` utilisé correctement).

### Test de correction
- Vérification systématique (`is_sorted`) à chaque étape du benchmark. Si l'algorithme échoue, le programme alertera l'utilisateur.
