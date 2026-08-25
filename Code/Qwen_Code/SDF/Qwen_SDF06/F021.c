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
    Ingredient *ings;
    struct Recipe *next;
} Recipe;

Recipe *head = NULL;

Recipe *find_recipe(const char *name) {
    for (Recipe *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) return r;
    }
    return NULL;
}

void add_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    r->name = strdup(name);
    if (!r->name) { free(r); return; }
    r->ings = NULL;
    r->next = head;
    head = r;
}

void add_ing(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    i->name = strdup(iname);
    i->note = strdup(note);
    if (!i->name || !i->note) { free(i->name); free(i->note); free(i); return; }
    i->next = r->ings;
    r->ings = i;
}

void remove_ing(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *curr = r->ings;
    Ingredient *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, iname) == 0) {
            if (prev) prev->next = curr->next;
            else r->ings = curr->next;
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
    Recipe *curr = head;
    Recipe *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Ingredient *i = curr->ings;
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
    for (Ingredient *i = r->ings; i; i = i->next) {
        printf("%s %s\n", i->name, i->note);
    }
}

void list_recipes() {
    for (Recipe *r = head; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

void free_all() {
    Recipe *r = head;
    while (r) {
        Recipe *next_r = r->next;
        Ingredient *i = r->ings;
        while (i) {
            Ingredient *next_i = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = next_i;
        }
        free(r->name);
        free(r);
        r = next_r;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %1023s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        
        if (strcmp(cmd, "RECIPE") == 0 && n >= 2) {
            add_recipe(arg1);
        } else if (strcmp(cmd, "ING") == 0 && n >= 4) {
            add_ing(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVEING") == 0 && n >= 3) {
            remove_ing(arg1, arg2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && n >= 2) {
            delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    free_all();
    return 0;
}