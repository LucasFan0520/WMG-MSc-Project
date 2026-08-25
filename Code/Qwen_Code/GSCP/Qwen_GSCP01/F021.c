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
    Recipe *curr = recipes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) exit(EXIT_FAILURE);
    r->name = strdup(name);
    r->ingredients = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) exit(EXIT_FAILURE);
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *curr = r->ingredients;
    Ingredient *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, iname) == 0) {
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

void delete_recipe(const char *name) {
    Recipe *curr = recipes;
    Recipe *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else recipes = curr->next;
            Ingredient *i = curr->ingredients;
            while (i) {
                Ingredient *next_i = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next_i;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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

void list_recipes() {
    Recipe *curr = recipes;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Recipe *curr = recipes;
    while (curr) {
        Recipe *next_r = curr->next;
        Ingredient *i = curr->ingredients;
        while (i) {
            Ingredient *next_i = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = next_i;
        }
        free(curr->name);
        free(curr);
        curr = next_r;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        
        if (strcmp(cmd, "RECIPE") == 0) {
            create_recipe(arg1);
        } else if (strcmp(cmd, "ING") == 0 && matched >= 3) {
            add_ingredient(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVEING") == 0 && matched >= 3) {
            remove_ingredient(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}