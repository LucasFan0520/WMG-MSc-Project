// filename: F021.c
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

static Recipe *recipes = NULL;

static void free_ingredients(Ingredient *ing) {
    while (ing) {
        Ingredient *tmp = ing;
        ing = ing->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

static void cleanup(void) {
    while (recipes) {
        Recipe *tmp = recipes;
        recipes = recipes->next;
        free(tmp->name);
        free_ingredients(tmp->ingredients);
        free(tmp);
    }
}

static Recipe *find_recipe(const char *name) {
    for (Recipe *r = recipes; r; r = r->next)
        if (strcmp(r->name, name) == 0)
            return r;
    return NULL;
}

static void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    r->name = strdup(name);
    if (!r->name) { free(r); return; }
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

static void ingredient_add(const char *recipe_name, const char *ing_name, const char *note) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *ing = malloc(sizeof(Ingredient));
    if (!ing) return;
    ing->name = strdup(ing_name);
    ing->note = strdup(note);
    if (!ing->name || !ing->note) {
        free(ing->name);
        free(ing->note);
        free(ing);
        return;
    }
    ing->next = r->ingredients;
    r->ingredients = ing;
}

static void ingredient_remove(const char *recipe_name, const char *ing_name) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *prev = NULL;
    for (Ingredient *ing = r->ingredients; ing; ing = ing->next) {
        if (strcmp(ing->name, ing_name) == 0) {
            if (prev) prev->next = ing->next;
            else r->ingredients = ing->next;
            free(ing->name);
            free(ing->note);
            free(ing);
            return;
        }
        prev = ing;
    }
}

static void recipe_delete(const char *name) {
    Recipe *prev = NULL;
    for (Recipe *r = recipes; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            free(r->name);
            free_ingredients(r->ingredients);
            free(r);
            return;
        }
        prev = r;
    }
}

static void recipe_show(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    for (Ingredient *ing = r->ingredients; ing; ing = ing->next)
        printf("%s %s\n", ing->name, ing->note);
}

static void recipe_list(void) {
    for (Recipe *r = recipes; r; r = r->next)
        printf("%s\n", r->name);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "RECIPE") == 0) {
            if (*arg1) recipe_create(arg1);
        } else if (strcmp(cmd, "ING") == 0) {
            if (*arg1 && *arg2) ingredient_add(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (*arg1 && *arg2) ingredient_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (*arg1) recipe_delete(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (*arg1) recipe_show(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            recipe_list();
        }
    }
    free(line);
    return 0;
}