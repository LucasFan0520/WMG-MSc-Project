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

Recipe *head = NULL, *tail = NULL;

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
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        head = tail = r;
    }
}

void add_ingredient(const char *name, const char *ingredient, const char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = malloc(strlen(ingredient) + 1);
    strcpy(i->name, ingredient);
    i->note = malloc(strlen(note) + 1);
    strcpy(i->note, note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void remove_ingredient(const char *name, const char *ingredient) {
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

void delete_recipe(const char *name) {
    Recipe *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Ingredient *i = curr->ingredients;
            while (i) {
                Ingredient *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
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
        Ingredient *i = r->ingredients;
        while (i) {
            Ingredient *nexti = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = nexti;
        }
        free(r->name);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = strtok(NULL, " ");
            char *ingredient = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && ingredient && note) add_ingredient(name, ingredient, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = strtok(NULL, " ");
            char *ingredient = strtok(NULL, " ");
            if (name && ingredient) remove_ingredient(name, ingredient);
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
    free(line);
    free_all();
    return 0;
}