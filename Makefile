# Nom de l'exécutable
NAME = OpenGL_Project

# Compilateur et options
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++17

# Bibliothèques et flags pour OpenGL
LDFLAGS = -lGL -lglfw -lpthread -ldl
INCLUDES = -Iinclude

# Répertoires
SRCDIR = src
OBJDIR = obj

# Fichiers source
SRCS = $(wildcard $(SRCDIR)/*.cpp)

# Fichiers objet
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Commandes
all: $(NAME)

# Règle pour créer l'exécutable à la racine
$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "Compilation terminée avec succès : $(NAME)"

# Règle pour compiler les fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<
	@echo "Compilation de $< en $@"

# Nettoyer les fichiers objets
clean:
	@rm -rf $(OBJDIR)
	@echo "Nettoyage des fichiers objets terminé"

# Nettoyer tout (fichiers objets + exécutable)
fclean: clean
	@rm -f $(NAME)
	@echo "Nettoyage complet terminé"

# Recompiler tout
re: fclean all
	@echo "Recompilation complète terminée"

.PHONY: all clean fclean re
