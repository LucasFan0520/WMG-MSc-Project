// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ing {
    char *name;
    char *note;
    struct Ing *next;
} Ing;

typedef struct Recipe {
    char *name;
    Ing *ings;
    struct Recipe *next;
} Recipe;

Recipe *head = NULL;

Recipe *find_recipe(char *name) {
    for (Recipe *r = head; r; r = r->next) {
        if (strcmp(r->name, name) == 0) return r;
    }
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

void add_ing(char *rname, char *iname, char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ing *i = malloc(sizeof(Ing));
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ing(char *rname, char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ing *prev = NULL;
    Ing *curr = r->ings;
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

void delete_recipe(char *name) {
    Recipe *prev = NULL;
    Recipe *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Ing *i = curr->ings;
            while (i) {
                Ing *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_recipe(char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    for (Ing *i = r->ings; i; i = i->next) {
        printf("%s %s\n", i->name, i->note);
    }
}

void list_recipes() {
    for (Recipe *r = head; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

int main() {
    char cmd[20], a[100], b[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "RECIPE") == 0) {
            scanf("%99s", a);
            create_recipe(a);
        } else if (strcmp(cmd, "ING") == 0) {
            scanf("%99s%99s", a, b);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_ing(a, b, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            scanf("%99s%99s", a, b);
            remove_ing(a, b);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            scanf("%99s", a);
            delete_recipe(a);
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%99s", a);
            show_recipe(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }
    return 0;
}