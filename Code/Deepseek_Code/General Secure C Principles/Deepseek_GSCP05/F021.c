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
    if (!r) return;
    r->name = strdup(name);
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(const char *recipe, const char *ing, const char *note) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    if (find_ingredient(r, ing)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    i->name = strdup(ing);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *recipe, const char *ing) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    Ingredient *prev = NULL, *cur = r->ingredients;
    while (cur) {
        if (strcmp(cur->name, ing) == 0) {
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
    Recipe *prev = NULL, *cur = recipes;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else recipes = cur->next;
            Ingredient *i = cur->ingredients;
            while (i) {
                Ingredient *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
            }
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s: ", i->name);
        char *s = i->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
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

void free_all(void) {
    Recipe *r = recipes;
    while (r) {
        Recipe *next = r->next;
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *inext = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = inext;
        }
        free(r->name);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *recipe = p;
            char *space = strchr(recipe, ' ');
            if (!space) continue;
            *space = '\0';
            char *ing = space + 1;
            while (*ing == ' ') ing++;
            space = strchr(ing, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            add_ingredient(recipe, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *p = line + 11;
            while (*p == ' ') p++;
            char *recipe = p;
            char *space = strchr(recipe, ' ');
            if (!space) continue;
            *space = '\0';
            char *ing = space + 1;
            while (*ing == ' ') ing++;
            remove_ingredient(recipe, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *p = line + 14;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}