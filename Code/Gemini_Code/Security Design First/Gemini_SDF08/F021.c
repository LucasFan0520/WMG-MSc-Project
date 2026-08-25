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

int main(void) {
    Recipe *recipes = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            Recipe *tmp = realloc(recipes, sizeof(Recipe) * (count + 1));
            if (tmp) {
                recipes = tmp;
                recipes[count].name = strdup(name);
                recipes[count].ingredients = NULL;
                recipes[count].ing_count = 0;
                if (recipes[count].name) {
                    count++;
                }
            }
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *r_name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *ing_name = p2;
                    char *note = s2 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(recipes[i].name, r_name) == 0) {
                            Ingredient *itmp = realloc(recipes[i].ingredients, sizeof(Ingredient) * (recipes[i].ing_count + 1));
                            if (itmp) {
                                recipes[i].ingredients = itmp;
                                recipes[i].ingredients[recipes[i].ing_count].ing_name = strdup(ing_name);
                                recipes[i].ingredients[recipes[i].ing_count].note = strdup(note);
                                if (recipes[i].ingredients[recipes[i].ing_count].ing_name && recipes[i].ingredients[recipes[i].ing_count].note) {
                                    recipes[i].ing_count++;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *r_name = p;
                char *ing_name = s1 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, r_name) == 0) {
                        int idx = -1;
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ingredients[j].ing_name, ing_name) == 0) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx != -1) {
                            free(recipes[i].ingredients[idx].ing_name);
                            free(recipes[i].ingredients[idx].note);
                            for (int j = idx; j < recipes[i].ing_count - 1; j++) {
                                recipes[i].ingredients[j] = recipes[i].ingredients[j + 1];
                            }
                            recipes[i].ing_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(recipes[idx].name);
                for (int j = 0; j < recipes[idx].ing_count; j++) {
                    free(recipes[idx].ingredients[j].ing_name);
                    free(recipes[idx].ingredients[j].note);
                }
                free(recipes[idx].ingredients);
                for (int i = idx; i < count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("  %s: %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].ing_name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    free(line);
    return 0;
}
