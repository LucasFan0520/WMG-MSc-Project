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
Recipe *tail = NULL;

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
    r->name = malloc(strlen(name)+1);
    strcpy(r->name, name);
    r->ingredients = NULL;
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void add_ingredient(const char *name, const char *ing, const char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = malloc(strlen(ing)+1);
    strcpy(i->name, ing);
    i->note = malloc(strlen(note)+1);
    strcpy(i->note, note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *name, const char *ing) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *prev = NULL;
    Ingredient *i = r->ingredients;
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
    Recipe *prev = NULL;
    Recipe *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = strtok(rest, " ");
            if (name) create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *ing = strtok(NULL, " ");
            if (!ing) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_ingredient(name, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *ing = strtok(NULL, " ");
            if (ing) remove_ingredient(name, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = strtok(rest, " ");
            if (name) show_recipe(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}