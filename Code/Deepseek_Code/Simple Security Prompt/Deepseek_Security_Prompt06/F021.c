// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ingredient {
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

void create_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(const char *name, const char *ingredient, const char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->note = malloc(strlen(ingredient) + 2 + strlen(note) + 1);
    sprintf(i->note, "%s %s", ingredient, note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ingredient(const char *name, const char *ingredient) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = r->ings, *prev = NULL;
    while (i) {
        if (strncmp(i->note, ingredient, strlen(ingredient)) == 0 &&
            (i->note[strlen(ingredient)] == ' ' || i->note[strlen(ingredient)] == '\0')) {
            if (prev) prev->next = i->next;
            else r->ings = i->next;
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
            Ingredient *i = r->ings;
            while (i) {
                Ingredient *tmp = i->next;
                free(i->note);
                free(i);
                i = tmp;
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
    Ingredient *i = r->ings;
    while (i) {
        printf("%s\n", i->note);
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

void free_all(void) {
    Recipe *r = recipes;
    while (r) {
        Recipe *tmp = r->next;
        free(r->name);
        Ingredient *i = r->ings;
        while (i) {
            Ingredient *ti = i->next;
            free(i->note);
            free(i);
            i = ti;
        }
        free(r);
        r = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], ingredient[256], note[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(rest, "%255s %255s", name, ingredient) == 2) {
                const char *n = rest + strlen(name) + 1 + strlen(ingredient) + 1;
                while (*n == ' ') n++;
                if (*n) add_ingredient(name, ingredient, n);
                else add_ingredient(name, ingredient, "");
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(rest, "%255s %255s", name, ingredient) == 2) {
                remove_ingredient(name, ingredient);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(rest, "%255s", name) == 1) show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}