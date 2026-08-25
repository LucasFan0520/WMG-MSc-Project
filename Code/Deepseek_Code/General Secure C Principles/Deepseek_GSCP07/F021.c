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
    Recipe *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_ingredients(Ingredient *i) {
    while (i) {
        Ingredient *tmp = i;
        i = i->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

void free_recipe(Recipe *r) {
    free(r->name);
    free_ingredients(r->ingredients);
    free(r);
}

void handle_recipe(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    if (find_recipe(name)) return;
    Recipe *new = malloc(sizeof(Recipe));
    new->name = strdup(name);
    new->ingredients = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Recipe *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_ing(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Recipe *r = find_recipe(name);
    if (!r) return;
    char *ingname = strtok(NULL, " ");
    if (!ingname) return;
    char *note = strtok(NULL, "");
    if (!note) note = "";
    Ingredient *new = malloc(sizeof(Ingredient));
    new->name = strdup(ingname);
    new->note = strdup(note);
    new->next = NULL;
    if (!r->ingredients) r->ingredients = new;
    else {
        Ingredient *last = r->ingredients;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_removeing(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Recipe *r = find_recipe(name);
    if (!r) return;
    char *ingname = strtok(NULL, " ");
    if (!ingname) return;
    Ingredient *i = r->ingredients, *prev = NULL;
    while (i) {
        if (strcmp(i->name, ingname) == 0) {
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

void handle_deleterecipe(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Recipe *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_recipe(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_show(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "RECIPE") == 0) handle_recipe(cmd);
        else if (strcmp(cmd, "ING") == 0) handle_ing(cmd);
        else if (strcmp(cmd, "REMOVEING") == 0) handle_removeing(cmd);
        else if (strcmp(cmd, "DELETERECIPE") == 0) handle_deleterecipe(cmd);
        else if (strcmp(cmd, "SHOW") == 0) handle_show(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Recipe *tmp = head;
        head = head->next;
        free_recipe(tmp);
    }
    return 0;
}