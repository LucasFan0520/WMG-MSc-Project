/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ingredient {
    char *name;
    char *note;
} Ingredient;

typedef struct Recipe {
    char *name;
    Ingredient *ingredients;
    size_t icount, icap;
} Recipe;

static Recipe *recipes = NULL;
static size_t rcount = 0, rcap = 0;

static Recipe *find_recipe(const char *name) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(recipes[i].name, name) == 0)
            return &recipes[i];
    }
    return NULL;
}

static void add_recipe(const char *name) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        recipes = realloc(recipes, rcap * sizeof(Recipe));
        if (!recipes) { perror("realloc"); exit(1); }
    }
    recipes[rcount].name = strdup(name);
    recipes[rcount].ingredients = NULL;
    recipes[rcount].icount = recipes[rcount].icap = 0;
    rcount++;
}

static void add_ingredient(Recipe *r, const char *ing, const char *note) {
    if (r->icount == r->icap) {
        r->icap = r->icap ? r->icap * 2 : 4;
        r->ingredients = realloc(r->ingredients, r->icap * sizeof(Ingredient));
        if (!r->ingredients) { perror("realloc"); exit(1); }
    }
    r->ingredients[r->icount].name = strdup(ing);
    r->ingredients[r->icount].note = strdup(note);
    r->icount++;
}

static void remove_ingredient(Recipe *r, const char *ing) {
    for (size_t i = 0; i < r->icount; i++) {
        if (strcmp(r->ingredients[i].name, ing) == 0) {
            free(r->ingredients[i].name);
            free(r->ingredients[i].note);
            for (size_t j = i; j < r->icount - 1; j++)
                r->ingredients[j] = r->ingredients[j + 1];
            r->icount--;
            return;
        }
    }
}

static void delete_recipe(const char *name) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(recipes[i].name, name) == 0) {
            free(recipes[i].name);
            for (size_t j = 0; j < recipes[i].icount; j++) {
                free(recipes[i].ingredients[j].name);
                free(recipes[i].ingredients[j].note);
            }
            free(recipes[i].ingredients);
            for (size_t j = i; j < rcount - 1; j++)
                recipes[j] = recipes[j + 1];
            rcount--;
            return;
        }
    }
}

static void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    for (size_t i = 0; i < r->icount; i++) {
        printf("%s %s\n", r->ingredients[i].name, r->ingredients[i].note);
    }
}

static void list_recipes(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%s\n", recipes[i].name);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            if (!find_recipe(name)) add_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *ing = strtok(NULL, " ");
            if (!ing) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            Recipe *r = find_recipe(name);
            if (r) add_ingredient(r, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *ing = strtok(NULL, " ");
            if (!ing) continue;
            Recipe *r = find_recipe(name);
            if (r) remove_ingredient(r, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, " ");
            if (name) show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    for (size_t i = 0; i < rcount; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].icount; j++) {
            free(recipes[i].ingredients[j].name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}