// F021.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    size_t ing_count;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Recipe *temp = realloc(recipes, (count + 1) * sizeof(Recipe));
                if (temp) {
                    recipes = temp;
                    recipes[count].name = strdup(name);
                    recipes[count].ingredients = NULL;
                    recipes[count].ing_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *ing = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Ingredient *temp = realloc(recipes[i].ingredients, (recipes[i].ing_count + 1) * sizeof(Ingredient));
                    if (temp) {
                        recipes[i].ingredients = temp;
                        recipes[i].ingredients[recipes[i].ing_count].name = strdup(ing);
                        recipes[i].ingredients[recipes[i].ing_count].note = strdup(note);
                        recipes[i].ing_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *ing = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        if (strcmp(recipes[i].ingredients[k].name, ing) == 0) {
                            free(recipes[i].ingredients[k].name);
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
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        free(recipes[i].ingredients[k].name);
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
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        printf("  %s: %s\n", recipes[i].ingredients[k].name, recipes[i].ingredients[k].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(recipes[i].name);
        for (size_t k = 0; k < recipes[i].ing_count; k++) {
            free(recipes[i].ingredients[k].name);
            free(recipes[i].ingredients[k].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    free(line);
    return 0;
}
