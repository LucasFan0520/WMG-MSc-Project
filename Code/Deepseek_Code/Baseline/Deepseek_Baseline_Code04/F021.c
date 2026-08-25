/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ingredient {
    char *ingredient;
    char *note;
    struct Ingredient *next;
} Ingredient;

typedef struct Recipe {
    char *name;
    Ingredient *ings;
    struct Recipe *next;
} Recipe;

Recipe *recipes = NULL;

Recipe *find_recipe(const char *name) {
    Recipe *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(const char *name, const char *ing, const char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->ingredient = strdup(ing);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ingredient(const char *name, const char *ing) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *prev = NULL, *cur = r->ings;
    while (cur) {
        if (strcmp(cur->ingredient, ing) == 0) {
            if (prev) prev->next = cur->next;
            else r->ings = cur->next;
            free(cur->ingredient);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_recipe(const char *name) {
    Recipe *prev = NULL, *cur = recipes;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else recipes = cur->next;
            free(cur->name);
            Ingredient *i = cur->ings;
            while (i) {
                Ingredient *tmp = i;
                i = i->next;
                free(tmp->ingredient);
                free(tmp->note);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    Ingredient *i = r->ings;
    while (i) {
        printf("%s %s\n", i->ingredient, i->note);
        i = i->next;
    }
}

void list_recipes(void) {
    Recipe *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_recipes(void) {
    while (recipes) {
        Recipe *tmp = recipes;
        recipes = recipes->next;
        free(tmp->name);
        Ingredient *i = tmp->ings;
        while (i) {
            Ingredient *ti = i;
            i = i->next;
            free(ti->ingredient);
            free(ti->note);
            free(ti);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], ing[256];
        int n = sscanf(line, "%s %s %s", cmd, name, ing);
        if (strcmp(cmd, "RECIPE") == 0) {
            if (n < 2) continue;
            add_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1 + strlen(ing) + 1;
            while (*p == ' ') p++;
            if (*p) add_ingredient(name, ing, p);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (n < 3) continue;
            remove_ingredient(name, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (n < 2) continue;
            delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (n < 2) continue;
            show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_recipes();
    return 0;
}