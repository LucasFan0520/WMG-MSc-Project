// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ingredient {
    char *name;
    char *note;
    struct ingredient *next;
};

struct recipe {
    char *name;
    struct ingredient *ings;
    struct recipe *next;
};

struct recipe *recipes = NULL;

struct recipe *find_recipe(char *name) {
    struct recipe *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void create_recipe(char *name) {
    if (find_recipe(name)) return;
    struct recipe *r = malloc(sizeof(struct recipe));
    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(char *name, char *ing, char *note) {
    struct recipe *r = find_recipe(name);
    if (!r) return;
    struct ingredient *i = r->ings;
    while (i) {
        if (strcmp(i->name, ing) == 0) return;
        i = i->next;
    }
    i = malloc(sizeof(struct ingredient));
    i->name = malloc(strlen(ing) + 1);
    strcpy(i->name, ing);
    i->note = malloc(strlen(note) + 1);
    strcpy(i->note, note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ingredient(char *name, char *ing) {
    struct recipe *r = find_recipe(name);
    if (!r) return;
    struct ingredient *i = r->ings, *prev = NULL;
    while (i) {
        if (strcmp(i->name, ing) == 0) {
            if (prev) prev->next = i->next;
            else r->ings = i->next;
            free(i->name);
            free(i->note);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void delete_recipe(char *name) {
    struct recipe *r = recipes, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            free(r->name);
            struct ingredient *i = r->ings;
            while (i) {
                struct ingredient *next = i->next;
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

void show_recipe(char *name) {
    struct recipe *r = find_recipe(name);
    if (r) {
        printf("%s\n", r->name);
        struct ingredient *i = r->ings;
        while (i) {
            printf("%s %s\n", i->name, i->note);
            i = i->next;
        }
    }
}

void list_recipes() {
    struct recipe *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "RECIPE") == 0 && n >= 2) {
            create_recipe(arg1);
        } else if (strcmp(cmd, "ING") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_ingredient(arg1, arg2, rest);
        } else if (strcmp(cmd, "REMOVEING") == 0 && n >= 3) {
            remove_ingredient(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && n >= 2) {
            delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    return 0;
}