// F021.c
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
    int r_count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                recipes = realloc(recipes, (r_count + 1) * sizeof(Recipe));
                recipes[r_count].name = strdup(name);
                recipes[r_count].ingredients = NULL;
                recipes[r_count].ing_count = 0;
                r_count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char r_name[256], i_name[256];
            int bytes = 0;
            if (sscanf(args, "%255s %255s%n", r_name, i_name, &bytes) == 2) {
                char *note = args + bytes;
                while (*note == ' ') note++;
                for (int i = 0; i < r_count; i++) {
                    if (strcmp(recipes[i].name, r_name) == 0) {
                        recipes[i].ingredients = realloc(recipes[i].ingredients, (recipes[i].ing_count + 1) * sizeof(Ingredient));
                        recipes[i].ingredients[recipes[i].ing_count].ing_name = strdup(i_name);
                        recipes[i].ingredients[recipes[i].ing_count].note = strdup(note);
                        recipes[i].ing_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char r_name[256], i_name[256];
            if (sscanf(args, "%255s %255s", r_name, i_name) == 2) {
                for (int i = 0; i < r_count; i++) {
                    if (strcmp(recipes[i].name, r_name) == 0) {
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ingredients[j].ing_name, i_name) == 0) {
                                free(recipes[i].ingredients[j].ing_name);
                                free(recipes[i].ingredients[j].note);
                                for (int k = j; k < recipes[i].ing_count - 1; k++) {
                                    recipes[i].ingredients[k] = recipes[i].ingredients[k + 1];
                                }
                                recipes[i].ing_count--;
                                recipes[i].ingredients = realloc(recipes[i].ingredients, recipes[i].ing_count * sizeof(Ingredient));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < r_count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        free(recipes[i].name);
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            free(recipes[i].ingredients[j].ing_name);
                            free(recipes[i].ingredients[j].note);
                        }
                        free(recipes[i].ingredients);
                        for (int j = i; j < r_count - 1; j++) {
                            recipes[j] = recipes[j + 1];
                        }
                        r_count--;
                        recipes = realloc(recipes, r_count * sizeof(Recipe));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < r_count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        printf("Recipe: %s\n", recipes[i].name);
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            printf("  %s: %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (int i = 0; i < r_count; i++) {
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
