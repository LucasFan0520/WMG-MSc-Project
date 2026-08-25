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

Recipe *head = NULL;

Recipe *find_recipe(const char *name) {
    Recipe *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void create_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->ingredients = NULL;
    r->next = head;
    head = r;
}

void add_ingredient(const char *recipe_name, const char *ing_name, const char *note) {
    Recipe *r = find_recipe(recipe_name);
    if (!r) return;
    Ingredient *i = r->ingredients;
    while (i) {
        if (strcmp(i->name, ing_name) == 0) return;
        i = i->next;
    }
    Ingredient *new = malloc(sizeof(Ingredient));
    new->name = malloc(strlen(ing_name) + 1);
    strcpy(new->name, ing_name);
    new->note = malloc(strlen(note) + 1);
    strcpy(new->note, note);
    new->next = r->ingredients;
    r->ingredients = new;
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
    Recipe *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->name);
            Ingredient *i = r->ingredients;
            while (i) {
                Ingredient *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
            }
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s:", i->name);
        for (char *c = i->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        i = i->next;
    }
}

void list_recipes(void) {
    Recipe *r = head;
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
    Recipe *r = head;
    while (r) {
        Recipe *next = r->next;
        free(r->name);
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *nexti = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = nexti;
        }
        free(r);
        r = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            create_recipe(p);
        } else if (strcmp(cmd, "ING") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *recipe = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_ingredient(recipe, ing, p);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *recipe = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            remove_ingredient(recipe, p);
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
    return 0;
}