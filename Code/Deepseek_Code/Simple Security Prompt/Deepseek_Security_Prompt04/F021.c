/* F021.c */
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
    Ingredient *ings;
    struct Recipe *next;
} Recipe;

Recipe *head = NULL;

Recipe *find_recipe(char *name) {
    for (Recipe *r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

void create_recipe(char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ings = NULL;
    r->next = head;
    head = r;
}

void add_ingredient(char *name, char *ing, char *note) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = strdup(ing);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ingredient(char *name, char *ing) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    Ingredient *prev = NULL;
    for (Ingredient *i = r->ings; i; i = i->next) {
        if (strcmp(i->name, ing) == 0) {
            if (prev) prev->next = i->next;
            else r->ings = i->next;
            free(i->name);
            free(i->note);
            free(i);
            return;
        }
        prev = i;
    }
}

void delete_recipe(char *name) {
    Recipe *prev = NULL;
    for (Recipe *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free(r->name);
            Ingredient *i = r->ings;
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
    }
}

void show_recipe(char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    for (Ingredient *i = r->ings; i; i = i->next)
        printf("%s %s\n", i->name, i->note);
}

void list_recipes(void) {
    for (Recipe *r = head; r; r = r->next)
        printf("%s\n", r->name);
}

void free_all(void) {
    Recipe *r = head;
    while (r) {
        Recipe *next = r->next;
        free(r->name);
        Ingredient *i = r->ings;
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "RECIPE") == 0 && n == 2) {
            create_recipe(arg1);
        } else if (strcmp(cmd, "ING") == 0 && n == 4) {
            add_ingredient(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVEING") == 0 && n == 3) {
            remove_ingredient(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && n == 2) {
            delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n == 2) {
            show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_recipes();
        }
    }
    free(line);
    free_all();
    return 0;
}