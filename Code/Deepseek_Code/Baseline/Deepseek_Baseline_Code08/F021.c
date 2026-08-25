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

Recipe* find_recipe(const char *name) {
    Recipe *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

Ingredient* find_ingredient(Recipe *r, const char *name) {
    Ingredient *cur = r->ingredients;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *new = malloc(sizeof(Recipe));
    new->name = strdup(name);
    new->ingredients = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Recipe *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void add_ingredient(const char *recipe, const char *ing, const char *note) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    if (find_ingredient(r, ing)) return;
    Ingredient *new = malloc(sizeof(Ingredient));
    new->name = strdup(ing);
    new->note = strdup(note);
    new->next = NULL;
    if (!r->ingredients) r->ingredients = new;
    else {
        Ingredient *cur = r->ingredients;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
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
    Recipe *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            Ingredient *i = cur->ingredients;
            while (i) {
                Ingredient *tmp = i;
                i = i->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
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
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void list_recipes() {
    Recipe *cur = head;
    while (cur) {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
}

void free_all() {
    Recipe *cur = head;
    while (cur) {
        Recipe *tmp = cur;
        cur = cur->next;
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) add_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *ing = strtok(NULL, " ");
            if (!ing) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_ingredient(name, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *ing = strtok(NULL, " ");
            if (ing) remove_ingredient(name, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(NULL, " ");
            if (name) show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}