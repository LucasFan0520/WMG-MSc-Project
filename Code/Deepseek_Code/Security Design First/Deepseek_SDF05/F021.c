// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    r->name = safe_strdup(name);
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

void ingredient_add(const char *recipe_name, const char *ing_name, const char *note) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *i = find_ingredient(r, ing_name);
    if (i) {
        free(i->note);
        i->note = safe_strdup(note);
        return;
    }
    Ingredient *new_i = malloc(sizeof(Ingredient));
    if (!new_i) return;
    new_i->name = safe_strdup(ing_name);
    new_i->note = safe_strdup(note);
    new_i->next = r->ingredients;
    r->ingredients = new_i;
}

void ingredient_remove(const char *recipe_name, const char *ing_name) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *prev = NULL, *cur = r->ingredients;
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

void recipe_delete(const char *name) {
    Recipe *prev = NULL, *cur = recipes;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else recipes = cur->next;
            free(cur->name);
            Ingredient *i = cur->ingredients;
            while (i) {
                Ingredient *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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

void free_all(void) {
    Recipe *r = recipes;
    while (r) {
        Recipe *next_r = r->next;
        free(r->name);
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *next_i = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = next_i;
        }
        free(r);
        r = next_r;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "RECIPE") == 0 && n >= 2) {
            recipe_create(arg1);
        } else if (strcmp(cmd, "ING") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            ingredient_add(arg1, arg2, p);
        } else if (strcmp(cmd, "REMOVEING") == 0 && n >= 3) {
            ingredient_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && n >= 2) {
            recipe_delete(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            recipe_show(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            recipe_list();
        }
    }
    free_all();
    return 0;
}