// F021.c
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

Recipe *find_recipe(const char *name) {
    Recipe *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Ingredient *find_ingredient(Recipe *r, const char *name) {
    Ingredient *i = r->ingredients;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void create_recipe(const char *name) {
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
    Ingredient *prev = NULL;
    Ingredient *i = r->ingredients;
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
    Recipe *prev = NULL;
    Recipe *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            Ingredient *i = r->ingredients;
            while (i) {
                Ingredient *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
            }
            free(r->name);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s:", i->name);
        for (char *s = i->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        i = i->next;
    }
}

void list_recipes() {
    Recipe *r = recipes;
    int first = 1;
    while (r) {
        if (!first) putchar(' ');
        printf("%s", r->name);
        first = 0;
        r = r->next;
    }
    putchar('\n');
}

void free_all() {
    Recipe *r = recipes;
    while (r) {
        Recipe *nextr = r->next;
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *nexti = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = nexti;
        }
        free(r->name);
        free(r);
        r = nextr;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "RECIPE") == 0) {
            create_recipe(rest);
        } else if (strcmp(cmd, "ING") == 0) {
            char rname[256], iname[256], note[4096];
            if (sscanf(rest, "%255s %255s %[^\n]", rname, iname, note) == 3) {
                add_ingredient(rname, iname, note);
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char rname[256], iname[256];
            if (sscanf(rest, "%255s %255s", rname, iname) == 2) {
                remove_ingredient(rname, iname);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            delete_recipe(rest);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_recipe(rest);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}