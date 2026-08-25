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
    Ingredient *ings;
    int ing_count;
    int ing_capacity;
} Recipe;

Recipe *recipes = NULL;
int recipe_count = 0;
int recipe_capacity = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < recipe_count; i++) {
        if (strcmp(recipes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_recipe(const char *name) {
    if (find_recipe(name) != -1) return;
    if (recipe_count >= recipe_capacity) {
        recipe_capacity = recipe_capacity == 0 ? 4 : recipe_capacity * 2;
        recipes = realloc(recipes, recipe_capacity * sizeof(Recipe));
    }
    recipes[recipe_count].name = strdup(name);
    recipes[recipe_count].ings = NULL;
    recipes[recipe_count].ing_count = 0;
    recipes[recipe_count].ing_capacity = 0;
    recipe_count++;
}

void add_ingredient(const char *r_name, const char *ing_name, const char *note) {
    int idx = find_recipe(r_name);
    if (idx == -1) return;
    Recipe *r = &recipes[idx];
    if (r->ing_count >= r->ing_capacity) {
        r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
        r->ings = realloc(r->ings, r->ing_capacity * sizeof(Ingredient));
    }
    r->ings[r->ing_count].ing_name = strdup(ing_name);
    r->ings[r->ing_count].note = strdup(note);
    r->ing_count++;
}

void remove_ingredient(const char *r_name, const char *ing_name) {
    int idx = find_recipe(r_name);
    if (idx == -1) return;
    Recipe *r = &recipes[idx];
    for (int i = 0; i < r->ing_count; i++) {
        if (strcmp(r->ings[i].ing_name, ing_name) == 0) {
            free(r->ings[i].ing_name);
            free(r->ings[i].note);
            for (int j = i; j < r->ing_count - 1; j++) {
                r->ings[j] = r->ings[j + 1];
            }
            r->ing_count--;
            break;
        }
    }
}

void delete_recipe(const char *name) {
    int idx = find_recipe(name);
    if (idx == -1) return;
    free(recipes[idx].name);
    for (int i = 0; i < recipes[idx].ing_count; i++) {
        free(recipes[idx].ings[i].ing_name);
        free(recipes[idx].ings[i].note);
    }
    free(recipes[idx].ings);
    for (int i = idx; i < recipe_count - 1; i++) {
        recipes[i] = recipes[i + 1];
    }
    recipe_count--;
}

void show_recipe(const char *name) {
    int idx = find_recipe(name);
    if (idx == -1) return;
    printf("RECIPE %s\n", recipes[idx].name);
    for (int i = 0; i < recipes[idx].ing_count; i++) {
        printf("ING %s %s\n", recipes[idx].ings[i].ing_name, recipes[idx].ings[i].note);
    }
}

void list_recipes(void) {
    for (int i = 0; i < recipe_count; i++) {
        printf("%s\n", recipes[i].name);
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = strtok(NULL, "");
            if (name) create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *r_name = strtok(NULL, " ");
            char *ing_name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (r_name && ing_name && note) add_ingredient(r_name, ing_name, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *r_name = strtok(NULL, " ");
            char *ing_name = strtok(NULL, "");
            if (r_name && ing_name) remove_ingredient(r_name, ing_name);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, "");
            if (name) show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free(line);
    for (int i = 0; i < recipe_count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].ing_name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
