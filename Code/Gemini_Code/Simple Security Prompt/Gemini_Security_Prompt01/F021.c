// F021.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    int ing_count;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "RECIPE") == 0 && args) {
            recipes = realloc(recipes, (count + 1) * sizeof(Recipe));
            if (!recipes) return 1;
            recipes[count].name = strdup(args);
            recipes[count].ingredients = NULL;
            recipes[count].ing_count = 0;
            count++;
        } else if (strcmp(cmd, "ING") == 0 && args) {
            char *rname = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *ing = space2 + 1;
                while (*ing == ' ') ing++;
                char *space3 = strchr(ing, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        int ic = recipes[i].ing_count;
                        recipes[i].ingredients = realloc(recipes[i].ingredients, (ic + 1) * sizeof(Ingredient));
                        if (!recipes[i].ingredients) return 1;
                        recipes[i].ingredients[ic].ing_name = strdup(ing);
                        recipes[i].ingredients[ic].note = strdup(note);
                        recipes[i].ing_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0 && args) {
            char *rname = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *ing = space2 + 1;
                while (*ing == ' ') ing++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ingredients[j].ing_name, ing) == 0) {
                                free(recipes[i].ingredients[j].ing_name);
                                free(recipes[i].ingredients[j].note);
                                for (int k = j; k < recipes[i].ing_count - 1; k++) {
                                    recipes[i].ingredients[k] = recipes[i].ingredients[k + 1];
                                }
                                recipes[i].ing_count--;
                                recipes[i].ingredients = realloc(recipes[i].ingredients, recipes[i].ing_count * sizeof(Ingredient));
                                if (recipes[i].ing_count > 0 && !recipes[i].ingredients) return 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, args) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ingredients[j].ing_name);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    for (int j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    recipes = realloc(recipes, count * sizeof(Recipe));
                    if (count > 0 && !recipes) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, args) == 0) {
                    printf("RECIPE %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("ING %s %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].ing_name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
