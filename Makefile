# Nom de l'exécutable
TARGET = main

# Nom du fichier source
SRC = src/main.c

# Options de compilation
CFLAGS = `pkg-config sdl3 --cflags`
LDFLAGS = `pkg-config sdl3 --libs`

# Règle par défaut
all: $(TARGET)

# Règle pour construire l'exécutable
$(TARGET): $(SRC)
	gcc -o $@ $< $(CFLAGS) $(LDFLAGS)

# Nettoyer les fichiers générés
clean:
	rm -f $(TARGET)

# Forcer la recompilation
rebuild: clean all