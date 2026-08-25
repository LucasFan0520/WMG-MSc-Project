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
    r->name = strdup(name);
    r->ings = NULL;
    r->next = head;
    head = r;
}

void add_ing(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ing(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *p = r->ings, *prev = NULL;
    while (p) {
        if (strcmp(p->name, iname) == 0) {
            if (prev) prev->next = p->next;
            else r->ings = p->next;
            free(p->name); free(p->note); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void delete_recipe(const char *name) {
    Recipe *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            Ingredient *i = p->ings;
            while (i) {
                Ingredient *ni = i->next;
                free(i->name); free(i->note); free(i);
                i = ni;
            }
            free(p->name); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
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

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000], c[8000];
        c[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "RECIPE") == 0) {
            sscanf(line, "%*s %s", a);
            add_recipe(a);
        } else if (strcmp(cmd, "ING") == 0) {
            sscanf(line, "%*s %s %s %[^\n]", a, b, c);
            add_ing(a, b, c);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            remove_ing(a, b);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_recipe(a);
        } else if (strcmp(cmd, "SHOW") == 0) {
            sscanf(line, "%*s %s", a);
            show_recipe(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    while (head) {
        Recipe *n = head->next;
        Ingredient *i = head->ings;
        while (i) {
            Ingredient *ni = i->next;
            free(i->name); free(i->note); free(i);
            i = ni;
        }
        free(head->name); free(head);
        head = n;
    }
    return 0;
}