# Projet-Programmation-C
# 📊 Sorting Algorithms Benchmark

Benchmark des performances de **5 algorithmes de tri** implémentés en **C**, avec visualisation des résultats en **Python** (`pandas` + `matplotlib`).

Les algorithmes sont comparés sur différentes tailles de tableaux et sur trois scénarios : données **aléatoires**, **déjà triées** et **triées en ordre inverse**.

---

## ✨ Fonctionnalités

- 5 algorithmes de tri écrits from scratch : sélection, bulles, insertion, rapide, fusion
- Génération de données avec **allocation dynamique** (`malloc` / `free`)
- Générateur pseudo-aléatoire **xorshift64** (reproductible grâce à une graine)
- Architecture **modulaire** (génération de données, tris, utilitaires, programme principal)
- Résultats exportés en CSV puis tracés avec Python

---

## 🗂️ Structure du projet

```
.
├── src/
│   ├── data_gen.h / data_gen.c   # Génération des tableaux (aléatoire, trié, inversé)
│   ├── sorts.h / sorts.c         # Les 5 algorithmes de tri
│   ├── utils.h / utils.c         # is_sorted(), copy_array()
│   └── main.c                    # Boucle de benchmark
├── data/
│   └── results.csv               # Résultats générés
├── scripts/
│   └── plot.py                   # Visualisation (pandas + matplotlib)
├── Makefile
└── README.md
```

---

## 🧮 Algorithmes implémentés

| Algorithme | Aléatoire | Trié | Inversé | Mémoire | Stable |
|---|---|---|---|---|---|
| Tri par sélection | O(n²) | O(n²) | O(n²) | O(1) | Non |
| Tri à bulles (arrêt anticipé) | O(n²) | O(n) | O(n²) | O(1) | Oui |
| Tri par insertion | O(n²) | O(n) | O(n²) | O(1) | Oui |
| Tri rapide (pivot médian de 3) | O(n log n) | O(n log n) | O(n log n) | O(log n) | Non |
| Tri par fusion | O(n log n) | O(n log n) | O(n log n) | O(n) | Oui |

### Choix d'implémentation

- **Tri rapide** : pivot médian de trois + partition de Hoare. La récursion se fait sur la plus petite partition et la plus grande est traitée par boucle, donc la profondeur de pile reste ≤ log₂(n) (pas de stack overflow sur de très grands tableaux).
- **Tri par fusion** : un seul tableau temporaire alloué au début et libéré à la fin, au lieu d'une allocation par fusion.
- **Tri à bulles** : un drapeau `swapped` arrête le tri dès qu'une passe ne produit aucun échange.
- **Générateur aléatoire** : `rand()` est évité car `RAND_MAX` vaut 32 767 sous Windows, ce qui provoque beaucoup de doublons sur de grands tableaux.

---

## ⚙️ Protocole de benchmark

- Tailles testées : de `N = 10 000` à `N = 10 000 000`
- Scénarios : **Aléatoire**, **Trié**, **Inversé**
- Les tris quadratiques (sélection, bulles, insertion) sont limités à `N ≤ 100 000` pour éviter des temps d'exécution excessifs
- Chaque tri est vérifié avec `is_sorted()` après exécution
- Chaque test s'exécute sur une **copie** du tableau original
- Mesure du temps haute précision (`QueryPerformanceCounter` sous Windows, `clock_gettime` ailleurs)

Format du fichier `data/results.csv` :

```
Taille,Scénario,Nom_Algo,Temps,PC_Config
```

---

## 🚀 Compilation et utilisation

### Prérequis

- Un compilateur C (`gcc` recommandé)
- `make`
- Python 3 avec `pandas` et `matplotlib` (pour les graphiques)

### Compiler

```bash
make          # compilation complète
make clean    # suppression des fichiers objets et de l'exécutable
```

Ou manuellement :

```bash
gcc -Wall -Wextra -O2 -std=c99 src/main.c src/data_gen.c src/sorts.c src/utils.c -o benchmark
```

### Lancer le benchmark

```bash
./benchmark
```

Le programme demande la configuration du PC (CPU, RAM, etc.), puis écrit les résultats dans `data/results.csv`.

### Générer les graphiques

```bash
pip install pandas matplotlib
python scripts/plot.py
```

Trois graphiques sont produits (Aléatoire, Trié, Inversé), avec la configuration du PC dans le titre.

---

## 🧪 Exemple d'utilisation des modules

```c
#include <stdlib.h>
#include "data_gen.h"
#include "sorts.h"

int main(void) {
    data_gen_seed(42);                     // reproductibilité
    int n = 100000;
    int* arr = generate_random_array(n);   // allocation dynamique
    if (arr == NULL) return 1;

    quick_sort(arr, n);

    free(arr);                             // à la charge de l'appelant
    return 0;
}
```

---

## ✅ Vérification

Les modules `data_gen` et `sorts` ont été validés avec :

- compilation `-Wall -Wextra` sans aucun warning ;
- AddressSanitizer + UBSan : aucune fuite mémoire ni accès hors limites ;
- les 5 tris sur des tailles de 1 à 5 000 et sur les 3 scénarios ;
- quick sort et merge sort sur 2 000 000 d'éléments.

---

## 🗺️ Avancement

- [x] Module génération de données (`data_gen`)
- [x] Module algorithmes de tri (`sorts`)
- [ ] Module utilitaires (`utils`)
- [ ] Programme principal (`main.c`)
- [ ] Makefile
- [ ] Script de visualisation (`plot.py`)
- [ ] Lancement du benchmark complet et graphiques finaux

---

