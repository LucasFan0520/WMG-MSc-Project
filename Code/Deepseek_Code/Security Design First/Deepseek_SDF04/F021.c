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
    r->next = NULL;
    if (!r->name) { free(r); return; }
    if (!head) head = r;
    else {
        Recipe *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
}

void add_ingredient(const char *recipe, const char *ing, const char *note) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    if (find_ingredient(r, ing)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    i->name = strdup(ing);
    i->note = strdup(note);
    i->next = NULL;
    if (!i->name || !i->note) {
        free(i->name); free(i->note); free(i);
        return;
    }
    if (!r->ingredients) r->ingredients = i;
    else {
        Ingredient *last = r->ingredients;
        while (last->next) last = last->next;
        last->next = i;
    }
}

void remove_ingredient(const char *recipe, const char *ing) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    Ingredient *i = r->ingredients, *prev = NULL;
    while (i) {
        if (strcmp(i->name, ing) == 0) {
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
    Recipe *r = head, *prev = NULL;
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
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void list_recipes(void) {
    Recipe *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    Recipe *r = head;
    while (r) {
        Recipe *next = r->next;
        free(r->name);
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *inext = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = inext;
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
        char cmd[64], name[256], ing[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", name, ing, note) == 3)
                add_ingredient(name, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, ing) == 2)
                remove_ingredient(name, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free(line);
    free_all();
    return 0;
}