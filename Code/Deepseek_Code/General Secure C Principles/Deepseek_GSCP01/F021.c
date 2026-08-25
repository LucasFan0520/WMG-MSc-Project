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

void add_ingredient(const char *name, const char *ingredient, const char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->ingredient = strdup(ingredient);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *name, const char *ingredient) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = r->ingredients, *prev = NULL;
    while (i && strcmp(i->ingredient, ingredient) != 0) {
        prev = i;
        i = i->next;
    }
    if (!i) return;
    if (prev) prev->next = i->next;
    else r->ingredients = i->next;
    free(i->ingredient);
    free(i->note);
    free(i);
}

void delete_recipe(const char *name) {
    Recipe *r = recipes, *prev = NULL;
    while (r && strcmp(r->name, name) != 0) {
        prev = r;
        r = r->next;
    }
    if (!r) return;
    if (prev) prev->next = r->next;
    else recipes = r->next;
    free(r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        Ingredient *next = i->next;
        free(i->ingredient);
        free(i->note);
        free(i);
        i = next;
    }
    free(r);
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s ", i->ingredient);
        for (char *c = i->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        i = i->next;
    }
}

void list_recipes(void) {
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

void free_all(void) {
    Recipe *r = recipes;
    while (r) {
        Recipe *next = r->next;
        free(r->name);
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *nexti = i->next;
            free(i->ingredient);
            free(i->note);
            free(i);
            i = nexti;
        }
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            add_recipe(p);
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256], ingredient[256];
            char *p = line + 4;
            if (sscanf(p, "%255s %255s", name, ingredient) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_ingredient(name, ingredient, p);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256], ingredient[256];
            if (sscanf(line + 10, "%255s %255s", name, ingredient) == 2)
                remove_ingredient(name, ingredient);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            delete_recipe(p);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            show_recipe(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    free(line);
    return 0;
}