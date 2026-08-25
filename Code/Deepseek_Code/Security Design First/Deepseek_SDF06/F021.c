// F021.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ingredient {
    char *name;
    char *note;
} Ingredient;

typedef struct Recipe {
    char *name;
    Ingredient *ings;
    size_t ing_count;
    size_t ing_cap;
} Recipe;

static Recipe *recipes = NULL;
static size_t recipe_count = 0;
static size_t recipe_cap = 0;

static ssize_t find_recipe(const char *name) {
    for (size_t i = 0; i < recipe_count; i++)
        if (strcmp(recipes[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static ssize_t find_ingredient(Recipe *r, const char *name) {
    for (size_t i = 0; i < r->ing_count; i++)
        if (strcmp(r->ings[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_ingredient(Recipe *r, const char *name, const char *note) {
    if (find_ingredient(r, name) != -1) return;
    if (r->ing_count == r->ing_cap) {
        r->ing_cap = r->ing_cap ? r->ing_cap * 2 : 4;
        Ingredient *new = realloc(r->ings, r->ing_cap * sizeof(Ingredient));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        r->ings = new;
    }
    r->ings[r->ing_count].name = strdup(name);
    r->ings[r->ing_count].note = strdup(note);
    if (!r->ings[r->ing_count].name || !r->ings[r->ing_count].note)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    r->ing_count++;
}

static void remove_ingredient(Recipe *r, const char *name) {
    ssize_t idx = find_ingredient(r, name);
    if (idx == -1) return;
    free(r->ings[idx].name);
    free(r->ings[idx].note);
    for (size_t i = idx; i < r->ing_count - 1; i++)
        r->ings[i] = r->ings[i + 1];
    r->ing_count--;
}

static void add_recipe(const char *name) {
    if (find_recipe(name) != -1) return;
    if (recipe_count == recipe_cap) {
        recipe_cap = recipe_cap ? recipe_cap * 2 : 4;
        Recipe *new = realloc(recipes, recipe_cap * sizeof(Recipe));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        recipes = new;
    }
    recipes[recipe_count].name = strdup(name);
    if (!recipes[recipe_count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    recipes[recipe_count].ings = NULL;
    recipes[recipe_count].ing_count = 0;
    recipes[recipe_count].ing_cap = 0;
    recipe_count++;
}

static void delete_recipe(const char *name) {
    ssize_t idx = find_recipe(name);
    if (idx == -1) return;
    free(recipes[idx].name);
    for (size_t i = 0; i < recipes[idx].ing_count; i++) {
        free(recipes[idx].ings[i].name);
        free(recipes[idx].ings[i].note);
    }
    free(recipes[idx].ings);
    for (size_t i = idx; i < recipe_count - 1; i++)
        recipes[i] = recipes[i + 1];
    recipe_count--;
}

static void show_recipe(const char *name) {
    ssize_t idx = find_recipe(name);
    if (idx == -1) return;
    printf("%s\n", recipes[idx].name);
    for (size_t i = 0; i < recipes[idx].ing_count; i++)
        printf("%s %s\n", recipes[idx].ings[i].name, recipes[idx].ings[i].note);
}

static void list_recipes(void) {
    for (size_t i = 0; i < recipe_count; i++)
        printf("%s\n", recipes[i].name);
}

static void free_all(void) {
    for (size_t i = 0; i < recipe_count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg1[256], arg2[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                add_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", name, arg1, arg2) == 3) {
                ssize_t idx = find_recipe(name);
                if (idx != -1)
                    add_ingredient(&recipes[idx], arg1, arg2);
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, arg1) == 2) {
                ssize_t idx = find_recipe(name);
                if (idx != -1)
                    remove_ingredient(&recipes[idx], arg1);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }

    free(line);
    free_all();
    return 0;
}