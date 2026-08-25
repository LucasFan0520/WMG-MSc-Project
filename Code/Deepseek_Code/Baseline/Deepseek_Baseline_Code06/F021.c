/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ingredient {
    char *name;
    char *note;
    struct Ingredient *next;
} Ingredient;

typedef struct Recipe {
    char *name;
    Ingredient *ingredients;
    struct Recipe *next;
} Recipe;

Recipe *recipes = NULL;

Recipe* find_recipe(const char *name) {
    Recipe *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Ingredient* find_ingredient(Recipe *r, const char *name) {
    Ingredient *i = r->ingredients;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void add_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(const char *recipe_name, const char *ing_name, const char *note) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    if (find_ingredient(r, ing_name)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = strdup(ing_name);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *recipe_name, const char *ing_name) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *i = r->ingredients, *prev = NULL;
    while (i) {
        if (strcmp(i->name, ing_name) == 0) {
            if (prev) prev->next = i->next;
            else r->ingredients = i->next;
            free(i->name);
            free(i->note);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void delete_recipe(const char *name) {
    Recipe *r = recipes, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            free(r->name);
            Ingredient *i = r->ingredients;
            while (i) {
                Ingredient *tmp = i;
                i = i->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void list_recipes() {
    Recipe *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all() {
    while (recipes) {
        Recipe *tmp = recipes;
        recipes = recipes->next;
        free(tmp->name);
        Ingredient *i = tmp->ingredients;
        while (i) {
            Ingredient *itmp = i;
            i = i->next;
            free(itmp->name);
            free(itmp->note);
            free(itmp);
        }
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], ing[256], note[4096];
        int n = sscanf(line, "%31s %255s %255s %[^\n]", cmd, name, ing, note);
        if (strcmp(cmd, "RECIPE") == 0 && n >= 2) {
            add_recipe(name);
        } else if (strcmp(cmd, "ING") == 0 && n >= 3) {
            add_ingredient(name, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0 && n >= 3) {
            remove_ingredient(name, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && n >= 2) {
            delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}