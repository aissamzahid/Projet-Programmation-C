# ---------- Makefile : Benchmark des algorithmes de tri ----------
CC      = gcc
CFLAGS  = -Wall -Wextra -O2

# Fichiers du projet (les .c de Aissam et Reda doivent exister pour "make")
SRC     = src/main.c src/data_gen.c src/array_io.c src/sorts.c src/utils.c
OBJ     = $(SRC:.c=.o)

# Nom de l'executable et commande de nettoyage selon le systeme
ifeq ($(OS),Windows_NT)
    TARGET = benchmark.exe
    RM     = del /Q /F
    FIX    = $(subst /,\,$(1))
else
    TARGET = benchmark
    RM     = rm -f
    FIX    = $(1)
endif

all: $(TARGET)

# Etape finale : assembler tous les .o en un executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Regle generale : transformer chaque .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test independant du module utils (ne necessite pas le code des autres)
test_utils: tests/test_utils.c src/utils.c
	$(CC) $(CFLAGS) -o test_utils tests/test_utils.c src/utils.c
	./test_utils

clean:
	-$(RM) $(call FIX,$(OBJ)) $(TARGET) test_utils test_utils.exe

.PHONY: all clean test_utils
