// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ing {
    char *name;
    char *note;
    struct ing *next;
} Ing;

typedef struct recipe {
    char *name;
    Ing *ings;
    struct recipe *next;
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

void add_ingredient(Recipe *r, const char *name, const char *note) {
    Ing *i = r->ings;
    while (i) {
        if (strcmp(i->name, name) == 0) return;
        i = i->next;
    }
    Ing *new = malloc(sizeof(Ing));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = r->ings;
    r->ings = new;
}

void remove_ingredient(Recipe *r, const char *name) {
    Ing *i = r->ings, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else r->ings = i->next;
            free(i->name);
            free(i->note);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void delete_recipe(const char *name) {
    Recipe *r = recipes, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            free(r->name);
            Ing *i = r->ings;
            while (i) { Ing *tmp = i; i = i->next; free(tmp->name); free(tmp->note); free(tmp); }
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
    Ing *i = r->ings;
    while (i) {
        printf("%s %s\n", i->name, i->note);
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                recipe_create(arg1);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s %[^\n]", arg1, arg2, arg3) == 3) {
                Recipe *r = find_recipe(arg1);
                if (r) add_ingredient(r, arg2, arg3);
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2) {
                Recipe *r = find_recipe(arg1);
                if (r) remove_ingredient(r, arg2);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    return 0;
}