/*
 * F021.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ingredient {
    char *name;
    char *note;
    struct ingredient *next;
} Ingredient;

typedef struct recipe {
    char *name;
    Ingredient *ingredients;
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

Ingredient *find_ingredient(Recipe *r, const char *name) {
    Ingredient *i = r->ingredients;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

void recipe_add_ingredient(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    if (find_ingredient(r, iname)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void recipe_remove_ingredient(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *prev = NULL, *i = r->ingredients;
    while (i) {
        if (strcmp(i->name, iname) == 0) {
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

void recipe_delete(const char *name) {
    Recipe *prev = NULL, *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            Ingredient *i = r->ingredients;
            while (i) {
                Ingredient *tmp = i;
                i = i->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(r->name);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void recipe_show(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void recipe_list(void) {
    Recipe *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256];
        if (sscanf(line, "%31s %255s %255s", cmd, arg1, arg2) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "RECIPE") == 0) {
            recipe_create(arg1);
        } else if (strcmp(cmd, "ING") == 0) {
            recipe_add_ingredient(arg1, arg2, rest + strlen(arg2) + 1);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            recipe_remove_ingredient(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            recipe_delete(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            recipe_show(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            recipe_list();
        }
    }
    return 0;
}