# Projet-Programmation-C
# Sorting Algorithms Benchmark

Benchmark des performances de **5 algorithmes de tri** implementes en **C**, avec visualisation des resultats en **Python** (Jupyter Notebook `plots.ipynb`).

Les algorithmes sont compares sur differentes tailles de tableaux et sur trois scenarios : donnees **aleatoires**, **deja triees** et **triees en ordre inverse**.

---

## Fonctionnalites

- 5 algorithmes de tri ecrits from scratch : selection, bulles, insertion, rapide, fusion
- Generation de donnees avec **allocation dynamique** (`malloc` / `free`)
- Generateur pseudo-aleatoire **xorshift64** (reproductible grace a une graine)
- Architecture **modulaire** (generation de donnees, tris, utilitaires, I/O tableaux, programme principal)
- **Persistance des datasets** : les tableaux generes sont sauvegardes en binaire (`.bin`) dans `data_generated/` et recharges automatiquement pour eviter une re-generation a chaque execution
- **Nom de fichier CSV dynamique** : le fichier de resultats est nomme `data/results_<slug>.csv` selon la configuration PC lue depuis `data/pc_config.txt`
- Resultats exportes en CSV puis visualises avec Python (Jupyter Notebook)
- **Benchmark en deux phases** : phase quadratique (10k-100k) et phase rapide (200k-10M)

---

## Structure du projet

```
.
+-- src/
|   +-- data_gen.h / data_gen.c   # Generation des tableaux (aleatoire, trie, inverse)
|   +-- sorts.h / sorts.c         # Les 5 algorithmes de tri
|   +-- array_io.h / array_io.c   # Sauvegarde/chargement binaire et texte des tableaux
|   +-- utils.h / utils.c         # is_sorted(), copy_array()
|   +-- main.c                    # Boucle de benchmark (2 phases)
+-- data/
|   +-- pc_config.txt             # Configuration PC (CPU, RAM...) a remplir manuellement
|   +-- results_<slug>.csv        # Resultats generes (nomme selon pc_config)
|   +-- *.png                     # Graphiques generes par le notebook
+-- data_generated/
|   +-- {random,sorted,reverse}_<N>.bin  # Datasets persistants (generes automatiquement)
+-- tests/
|   +-- test_utils.c              # Tests unitaires du module utils
+-- scripts/
|   +-- plots.ipynb               # Visualisation interactive (Jupyter Notebook)
+-- Makefile
+-- README.md
```

---

## Algorithmes implementes

| Algorithme | Aleatoire | Trie | Inverse | Memoire | Stable |
|---|---|---|---|---|---|
| Tri par selection | O(n2) | O(n2) | O(n2) | O(1) | Non |
| Tri a bulles (arret anticipe) | O(n2) | O(n) | O(n2) | O(1) | Oui |
| Tri par insertion | O(n2) | O(n) | O(n2) | O(1) | Oui |
| Tri rapide (pivot median de 3) | O(n log n) | O(n log n) | O(n log n) | O(log n) | Non |
| Tri par fusion | O(n log n) | O(n log n) | O(n log n) | O(n) | Oui |

### Choix d'implementation

- **Tri rapide** : pivot median de trois + partition de Hoare. La recursion se fait sur la plus petite partition et la plus grande est traitee par boucle, donc la profondeur de pile reste <= log2(n) (pas de stack overflow sur de tres grands tableaux).
- **Tri par fusion** : un seul tableau temporaire alloue au debut et libere a la fin, au lieu d'une allocation par fusion.
- **Tri a bulles** : un drapeau `swapped` arrete le tri des qu'une passe ne produit aucun echange.
- **Generateur aleatoire** : `rand()` est evite car `RAND_MAX` vaut 32 767 sous Windows, ce qui provoque beaucoup de doublons sur de grands tableaux.

---

## Protocole de benchmark

Le benchmark se deroule en **deux phases** :

| Phase | Algorithmes | Plage de N | Pas |
|---|---|---|---|
| Phase 1 | Tous (y compris quadratiques) | 10 000 -> 100 000 | 10 000 |
| Phase 2 | Quick Sort & Merge Sort uniquement | 200 000 -> 10 000 000 | 100 000 |

- Scenarios : **Aleatoire**, **Trie**, **Inverse**
- Chaque tri est verifie avec `is_sorted()` apres execution
- Chaque test s'execute sur une **copie** du tableau original
- Mesure du temps haute precision (`QueryPerformanceCounter` sous Windows, `clock_gettime` ailleurs)
- Les datasets sont **persistes** dans `data_generated/` (fichiers `.bin`) et recharges automatiquement

### Configuration PC

La configuration du PC est lue depuis `data/pc_config.txt` (premiere ligne non-vide).
Elle est integree dans chaque ligne du CSV et dans le titre des graphiques.
Les virgules sont remplacees par des points-virgules pour ne pas corrompre le format CSV.

Format du fichier `data/results_<slug>.csv` :

```
Taille,Scenario,Nom_Algo,Temps,PC_Config
```

Le nom du fichier CSV est construit a partir du contenu de `pc_config.txt` (espaces -> `_`, caracteres speciaux supprimes).

---

## Compilation et utilisation

### Prerequis

- Un compilateur C (`gcc` recommande)
- `make`
- Python 3 avec `pandas`, `matplotlib` et `jupyter` (pour le notebook)

### Compiler

```bash
make          # compilation complete
make clean    # suppression des fichiers objets et de l'executable
```

Ou manuellement :

```bash
gcc -Wall -Wextra -O2 -std=c99 src/main.c src/data_gen.c src/array_io.c src/sorts.c src/utils.c -o benchmark
```

> Note : le module `array_io` est desormais requis a la compilation (il etait absent de la commande manuelle dans la version precedente du README).

### Configurer le PC (optionnel mais recommande)

Creer/modifier le fichier `data/pc_config.txt` et y ecrire sa configuration sur une ligne :

```
Intel Core i7-12700K; 32 Go RAM; Windows 11
```

Si ce fichier est absent, la valeur `Configuration inconnue` est utilisee.

### Lancer le benchmark

```bash
./benchmark        # Linux/macOS
./benchmark.exe    # Windows
```

Le programme detecte la configuration depuis `data/pc_config.txt`, puis ecrit les resultats dans `data/results_<slug>.csv`.

### Generer les graphiques

```bash
pip install pandas matplotlib jupyter
jupyter notebook scripts/plots.ipynb
```

Le notebook `plots.ipynb` contient plusieurs visualisations :
- Temps de tri par scenario (Aleatoire, Trie, Inverse)
- Impact du scenario sur les performances
- Comparaison multi-configurations PC
- Heatmap de l'impact du scenario
- Temps normalise

---

## Exemple d'utilisation des modules

```c
#include <stdlib.h>
#include "data_gen.h"
#include "sorts.h"

int main(void) {
    data_gen_seed(42);                     // reproductibilite
    int n = 100000;
    int* arr = generate_random_array(n);   // allocation dynamique
    if (arr == NULL) return 1;

    quick_sort(arr, n);

    free(arr);                             // a la charge de l'appelant
    return 0;
}
```

### Utilisation de `array_io`

```c
#include "array_io.h"

/* Sauvegarde un tableau en binaire */
save_array("data_generated/random_1000.bin", arr, 1000);

/* Chargement depuis un fichier .bin */
int loaded_n = 0;
int* arr = load_array("data_generated/random_1000.bin", &loaded_n);
free(arr);

/* Genere et sauvegarde les 3 scenarios pour N=1000 */
save_dataset("data_generated", 1000);
/* -> data_generated/random_1000.bin  */
/* -> data_generated/sorted_1000.bin  */
/* -> data_generated/reverse_1000.bin */
```

---

## Tests unitaires

```bash
make test_utils
```

Le fichier `tests/test_utils.c` teste les fonctions `is_sorted()` et `copy_array()` :
- tableau trie, inverse, valeurs egales, erreur au milieu, un seul element, tableau vide
- copie identique, independance de la copie par rapport a l'original

---

## Verification

Les modules `data_gen`, `sorts`, `utils` et `array_io` ont ete valides avec :

- compilation `-Wall -Wextra` sans aucun warning ;
- AddressSanitizer + UBSan : aucune fuite memoire ni acces hors limites ;
- les 5 tris sur des tailles de 1 a 5 000 et sur les 3 scenarios ;
- quick sort et merge sort sur 2 000 000 d'elements ;
- tests unitaires automatises via `make test_utils`.

---

## Avancement

- [x] Module generation de donnees (`data_gen`)
- [x] Module algorithmes de tri (`sorts`)
- [x] Module utilitaires (`utils`)
- [x] Module I/O tableaux (`array_io`) -- sauvegarde/chargement binaire et texte, generation de datasets
- [x] Programme principal (`main.c`) -- benchmark 2 phases, CSV dynamique, lecture pc_config
- [x] Makefile -- cibles `all`, `clean`, `test_utils`, compatible Windows/Linux
- [x] Script de visualisation (`scripts/plots.ipynb`) -- notebook Jupyter multi-graphiques
- [x] Tests unitaires (`tests/test_utils.c`)
- [x] Lancement du benchmark complet et graphiques finaux
