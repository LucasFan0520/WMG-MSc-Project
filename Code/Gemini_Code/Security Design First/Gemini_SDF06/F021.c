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
    size_t ing_count;
    size_t ing_capacity;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t rec_count = 0;
    size_t rec_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            if (rec_count >= rec_capacity) {
                size_t new_cap = rec_capacity == 0 ? 4 : rec_capacity * 2;
                Recipe *new_rec = realloc(recipes, new_cap * sizeof(Recipe));
                if (!new_rec) continue;
                recipes = new_rec;
                rec_capacity = new_cap;
            }
            char *rn = strdup(name);
            if (rn) {
                recipes[rec_count].name = rn;
                recipes[rec_count].ingredients = NULL;
                recipes[rec_count].ing_count = 0;
                recipes[rec_count].ing_capacity = 0;
                rec_count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *ing = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*name == '\0' || *ing == '\0') continue;

            for (size_t i = 0; i < rec_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    if (r->ing_count >= r->ing_capacity) {
                        size_t new_cap = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                        Ingredient *new_ing = realloc(r->ingredients, new_cap * sizeof(Ingredient));
                        if (!new_ing) break;
                        r->ingredients = new_ing;
                        r->ing_capacity = new_cap;
                    }
                    char *in = strdup(ing);
                    char *nt = strdup(note);
                    if (in && nt) {
                        r->ingredients[r->ing_count].ing_name = in;
                        r->ingredients[r->ing_count].note = nt;
                        r->ing_count++;
                    } else {
                        free(in);
                        free(nt);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *ing = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0' || *ing == '\0') continue;

            for (size_t i = 0; i < rec_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    for (size_t j = 0; j < r->ing_count; j++) {
                        if (strcmp(r->ingredients[j].ing_name, ing) == 0) {
                            free(r->ingredients[j].ing_name);
                            free(r->ingredients[j].note);
                            for (size_t k = j; k < r->ing_count - 1; k++) {
                                r->ingredients[k] = r->ingredients[k + 1];
                            }
                            r->ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < rec_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ingredients[j].ing_name);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    for (size_t j = i; j < rec_count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    rec_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < rec_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        printf("%s - %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < rec_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }

    for (size_t i = 0; i < rec_count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].ing_name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    free(line);
    return 0;
}
