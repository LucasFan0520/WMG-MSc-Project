// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ingredient { char *name; char *note; struct Ingredient *next; } Ingredient;
typedef struct Recipe { char *name; Ingredient *ings; struct Recipe *next; } Recipe;
Recipe *recipes = NULL;

Recipe *find_recipe(const char *name) {
    for (Recipe *r = recipes; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

Ingredient *find_ingredient(Recipe *r, const char *name) {
    for (Ingredient *i = r->ings; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    r->name = strdup(name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void ing_add(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    if (find_ingredient(r, iname)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ing(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *prev = NULL;
    for (Ingredient *i = r->ings; i; prev = i, i = i->next) {
        if (strcmp(i->name, iname) == 0) {
            if (prev) prev->next = i->next; else r->ings = i->next;
            free(i->name); free(i->note); free(i);
            return;
        }
    }
}

void delete_recipe(const char *name) {
    Recipe *prev = NULL;
    for (Recipe *r = recipes; r; prev = r, r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next; else recipes = r->next;
            free(r->name);
            while (r->ings) { Ingredient *i = r->ings; r->ings = i->next; free(i->name); free(i->note); free(i); }
            free(r);
            return;
        }
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->name);
    for (Ingredient *i = r->ings; i; i = i->next)
        printf("%s %s\n", i->name, i->note);
}

void list_recipes(void) {
    for (Recipe *r = recipes; r; r = r->next)
        printf("%s\n", r->name);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) recipe_create(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char rname[256], iname[256], note[4096];
            if (sscanf(rest, "%255s %255s", rname, iname) == 2) {
                char *n = rest + strlen(rname) + 1 + strlen(iname);
                while (*n == ' ') n++;
                strcpy(note, n);
                ing_add(rname, iname, note);
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char rname[256], iname[256];
            if (sscanf(rest, "%255s %255s", rname, iname) == 2) remove_ing(rname, iname);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    while (recipes) { Recipe *r = recipes; recipes = r->next; free(r->name); while (r->ings) { Ingredient *i = r->ings; r->ings = i->next; free(i->name); free(i->note); free(i); } free(r); }
    return 0;
}