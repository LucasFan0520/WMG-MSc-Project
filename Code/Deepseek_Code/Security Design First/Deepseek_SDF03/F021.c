// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_ING 64
#define MAX_NOTE 4096

typedef struct Ingredient {
    char name[MAX_ING];
    char note[MAX_NOTE];
    struct Ingredient *next;
} Ingredient;

typedef struct Recipe {
    char name[MAX_NAME];
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

void free_ingredients(Ingredient *i) {
    while (i) {
        Ingredient *tmp = i;
        i = i->next;
        free(tmp);
    }
}

void free_all() {
    Recipe *r = head;
    while (r) {
        Recipe *tmp = r;
        free_ingredients(r->ingredients);
        r = r->next;
        free(tmp);
    }
    head = NULL;
}

void handle_recipe(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "RECIPE %63s", name) != 1) return;
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    strcpy(r->name, name);
    r->ingredients = NULL;
    r->next = head;
    head = r;
}

void handle_ing(char *line) {
    char name[MAX_NAME], ing[MAX_ING], note[MAX_NOTE];
    int n = sscanf(line, "ING %63s %63s %4095[^\n]", name, ing, note);
    if (n < 3) return;
    Recipe *r = find_recipe(name);
    if (!r) return;
    if (find_ingredient(r, ing)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    strcpy(i->name, ing);
    strcpy(i->note, note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void handle_removeing(char *line) {
    char name[MAX_NAME], ing[MAX_ING];
    if (sscanf(line, "REMOVEING %63s %63s", name, ing) != 2) return;
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = r->ingredients, *prev = NULL;
    while (i) {
        if (strcmp(i->name, ing) == 0) {
            if (prev) prev->next = i->next;
            else r->ingredients = i->next;
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void handle_deleterecipe(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETERECIPE %63s", name) != 1) return;
    Recipe *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_ingredients(r->ingredients);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_show(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "SHOW %63s", name) != 1) return;
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void handle_list() {
    Recipe *r = head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "RECIPE ", 7) == 0) handle_recipe(line);
        else if (strncmp(line, "ING ", 4) == 0) handle_ing(line);
        else if (strncmp(line, "REMOVEING ", 10) == 0) handle_removeing(line);
        else if (strncmp(line, "DELETERECIPE ", 13) == 0) handle_deleterecipe(line);
        else if (strncmp(line, "SHOW ", 5) == 0) handle_show(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}