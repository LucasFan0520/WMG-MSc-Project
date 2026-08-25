/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_ingredients(Ingredient *i) {
    while (i) {
        Ingredient *tmp = i;
        i = i->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

void free_recipe(Recipe *r) {
    free(r->name);
    free_ingredients(r->ingredients);
    free(r);
}

void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

void ingredient_add(const char *name, const char *ingredient, const char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = strdup(ingredient);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void ingredient_remove(const char *name, const char *ingredient) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *prev = NULL, *curr = r->ingredients;
    while (curr) {
        if (strcmp(curr->name, ingredient) == 0) {
            if (prev) prev->next = curr->next;
            else r->ingredients = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void recipe_delete(const char *name) {
    Recipe *prev = NULL, *curr = recipes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else recipes = curr->next;
            free_recipe(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], ingredient[256], note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "RECIPE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) recipe_create(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, ingredient) != 2) continue;
            p += strlen(name) + strlen(ingredient) + 2;
            while (*p == ' ') p++;
            ingredient_add(name, ingredient, p);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, ingredient) == 2) ingredient_remove(name, ingredient);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) recipe_delete(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) recipe_show(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            recipe_list();
        }
    }

    while (recipes) {
        Recipe *tmp = recipes;
        recipes = recipes->next;
        free_recipe(tmp);
    }
    return 0;
}