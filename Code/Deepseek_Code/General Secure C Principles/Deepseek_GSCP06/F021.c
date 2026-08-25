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
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = strdup(ing_name);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *recipe_name, const char *ing_name) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *cur = r->ingredients, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, ing_name) == 0) {
            if (prev) prev->next = cur->next;
            else r->ingredients = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Recipe *cur = recipes, *prev = NULL;
    while (cur) {
        if (cur == r) {
            if (prev) prev->next = cur->next;
            else recipes = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
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

void list_recipes(void) {
    Recipe *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void cleanup_recipes(void) {
    Recipe *r = recipes;
    while (r) {
        Recipe *tmp = r;
        r = r->next;
        free(tmp->name);
        Ingredient *i = tmp->ingredients;
        while (i) {
            Ingredient *ti = i;
            i = i->next;
            free(ti->name);
            free(ti->note);
            free(ti);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024], arg3[2048];
        int n = sscanf(line, "%31s %255s %1023s %2047s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            add_recipe(arg1);
        } else if (strcmp(cmd, "ING") == 0 && n >= 3) {
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_ingredient(arg1, arg2, note);
        } else if (strcmp(cmd, "REMOVEING") == 0 && n >= 3) {
            remove_ingredient(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    cleanup_recipes();
    return 0;
}