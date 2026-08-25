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

void free_ings(Ing *i) {
    while (i) { Ing *t = i; i = i->next; free(t->name); free(t->note); free(t); }
}

void free_list() {
    while (head) {
        Recipe *t = head; head = head->next;
        free(t->name); free_ings(t->ings); free(t);
    }
}

Recipe *find_recipe(const char *name) {
    for (Recipe *r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], ing[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            if (find_recipe(name)) continue;
            Recipe *r = malloc(sizeof(Recipe));
            r->name = strdup(name); r->ings = NULL; r->next = head; head = r;
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", name, ing, note) < 2) continue;
            Recipe *r = find_recipe(name);
            if (r) {
                Ing *i = malloc(sizeof(Ing));
                i->name = strdup(ing); i->note = strdup(note); i->next = r->ings; r->ings = i;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, ing) != 2) continue;
            Recipe *r = find_recipe(name);
            if (r) {
                Ing *prev = NULL, *curr = r->ings;
                while (curr) {
                    if (strcmp(curr->name, ing) == 0) {
                        Ing *tmp = curr; curr = curr->next;
                        if (prev) prev->next = curr; else r->ings = curr;
                        free(tmp->name); free(tmp->note); free(tmp);
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Recipe *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Recipe *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free_ings(tmp->ings); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Recipe *r = find_recipe(name);
            if (r) {
                printf("%s:\n", r->name);
                for (Ing *i = r->ings; i; i = i->next) printf("  %s %s\n", i->name, i->note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Recipe *r = head; r; r = r->next) printf("%s\n", r->name);
        }
    }
    free_list();
    return 0;
}