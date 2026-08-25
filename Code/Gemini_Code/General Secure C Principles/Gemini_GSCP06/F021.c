// F021.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    size_t ing_count;
    size_t ing_capacity;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Recipe *next = realloc(recipes, capacity * sizeof(Recipe));
                if (!next) break;
                recipes = next;
            }
            recipes[count].name = strdup(name);
            recipes[count].ingredients = NULL;
            recipes[count].ing_count = 0;
            recipes[count].ing_capacity = 0;
            count++;
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *ing = rest;
                    char *note = sp2 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(recipes[i].name, name) == 0) {
                            if (recipes[i].ing_count >= recipes[i].ing_capacity) {
                                recipes[i].ing_capacity = recipes[i].ing_capacity == 0 ? 4 : recipes[i].ing_capacity * 2;
                                Ingredient *nxt = realloc(recipes[i].ingredients, recipes[i].ing_capacity * sizeof(Ingredient));
                                if (!nxt) break;
                                recipes[i].ingredients = nxt;
                            }
                            recipes[i].ingredients[recipes[i].ing_count].ingredient = strdup(ing);
                            recipes[i].ingredients[recipes[i].ing_count].note = strdup(note);
                            recipes[i].ing_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *args = line + 10;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *ing = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        for (size_t k = 0; k < recipes[i].ing_count; k++) {
                            if (strcmp(recipes[i].ingredients[k].ingredient, ing) == 0) {
                                free(recipes[i].ingredients[k].ingredient);
                                free(recipes[i].ingredients[k].note);
                                for (size_t j = k; j < recipes[i].ing_count - 1; j++) {
                                    recipes[i].ingredients[j] = recipes[i].ingredients[j + 1];
                                }
                                recipes[i].ing_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        free(recipes[i].ingredients[k].ingredient);
                        free(recipes[i].ingredients[k].note);
                    }
                    free(recipes[i].ingredients);
                    for (size_t j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("%s\n", recipes[i].name);
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        printf("%s %s\n", recipes[i].ingredients[k].ingredient, recipes[i].ingredients[k].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(recipes[i].name);
        for (size_t k = 0; k < recipes[i].ing_count; k++) {
            free(recipes[i].ingredients[k].ingredient);
            free(recipes[i].ingredients[k].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
