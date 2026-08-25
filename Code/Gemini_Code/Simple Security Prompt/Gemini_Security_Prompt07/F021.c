// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int ing_count;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                recipes = realloc(recipes, (count + 1) * sizeof(Recipe));
                recipes[count].name = strdup(name);
                recipes[count].ings = NULL;
                recipes[count].ing_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = strtok(NULL, " ");
            char *ing_name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && ing_name && note) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        recipes[i].ings = realloc(recipes[i].ings, (recipes[i].ing_count + 1) * sizeof(Ingredient));
                        recipes[i].ings[recipes[i].ing_count].ingredient = strdup(ing_name);
                        recipes[i].ings[recipes[i].ing_count].note = strdup(note);
                        recipes[i].ing_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = strtok(NULL, " ");
            char *ing_name = strtok(NULL, " ");
            if (name && ing_name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ings[j].ingredient, ing_name) == 0) {
                                free(recipes[i].ings[j].ingredient);
                                free(recipes[i].ings[j].note);
                                for (int k = j; k < recipes[i].ing_count - 1; k++) {
                                    recipes[i].ings[k] = recipes[i].ings[k + 1];
                                }
                                recipes[i].ing_count--;
                                if (recipes[i].ing_count == 0) {
                                    free(recipes[i].ings);
                                    recipes[i].ings = NULL;
                                } else {
                                    recipes[i].ings = realloc(recipes[i].ings, recipes[i].ing_count * sizeof(Ingredient));
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        free(recipes[i].name);
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            free(recipes[i].ings[j].ingredient);
                            free(recipes[i].ings[j].note);
                        }
                        free(recipes[i].ings);
                        for (int j = i; j < count - 1; j++) {
                            recipes[j] = recipes[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(recipes);
                    recipes = NULL;
                } else {
                    recipes = realloc(recipes, count * sizeof(Recipe));
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        printf("Recipe: %s\n", recipes[i].name);
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            printf("  %s: %s\n", recipes[i].ings[j].ingredient, recipes[i].ings[j].note);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
