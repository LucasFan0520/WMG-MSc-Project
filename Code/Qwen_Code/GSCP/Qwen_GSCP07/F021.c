// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ing {
    char *name;
    char *note;
    struct Ing *next;
} Ing;

typedef struct Recipe {
    char *name;
    Ing *ings;
    struct Recipe *next;
} Recipe;

Recipe *recipes = NULL;

Recipe *find_recipe(const char *name) {
    for (Recipe *r = recipes; r; r = r->next) {
        if (strcmp(r->name, name) == 0) return r;
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

void add_ing(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ing *i = malloc(sizeof(Ing));
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ing(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ing **p = &r->ings;
    while (*p) {
        if (strcmp((*p)->name, iname) == 0) {
            Ing *tmp = *p;
            *p = tmp->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void delete_recipe(const char *name) {
    Recipe **p = &recipes;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Recipe *tmp = *p;
            *p = tmp->next;
            Ing *i = tmp->ings;
            while (i) {
                Ing *next_i = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next_i;
            }
            free(tmp->name);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    for (Ing *i = r->ings; i; i = i->next) {
        printf("%s %s\n", i->name, i->note);
    }
}

void list_recipes() {
    for (Recipe *r = recipes; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

void cleanup() {
    while (recipes) {
        Recipe *tmp = recipes->next;
        Ing *i = recipes->ings;
        while (i) {
            Ing *next_i = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = next_i;
        }
        free(recipes->name);
        free(recipes);
        recipes = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) add_recipe(a1);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) add_ing(a1, a2, t + 1); } }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) remove_ing(a1, a2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_recipe(a1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) show_recipe(a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    cleanup();
    return 0;
}