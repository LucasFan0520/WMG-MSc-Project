// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

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

Recipe *recipes = NULL;

Recipe* find_recipe(const char *name) {
    for (Recipe *r = recipes; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            if (find_recipe(name)) continue;
            Recipe *r = malloc(sizeof(Recipe));
            if (!r) continue;
            r->name = strdup(name);
            if (!r->name) { free(r); continue; }
            r->ings = NULL;
            r->next = recipes;
            recipes = r;
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256], ing_name[256];
            int n = 0;
            if (sscanf(rest, "%255s %255s%n", name, ing_name, &n) != 2) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            Recipe *r = find_recipe(name);
            if (!r) continue;
            Ing *i = malloc(sizeof(Ing));
            if (!i) continue;
            i->name = strdup(ing_name);
            i->note = strdup(note);
            if (!i->name || !i->note) { free(i->name); free(i->note); free(i); continue; }
            i->next = r->ings;
            r->ings = i;
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256], ing_name[256];
            if (sscanf(rest, "%255s %255s", name, ing_name) != 2) continue;
            Recipe *r = find_recipe(name);
            if (!r) continue;
            Ing *prev = NULL;
            for (Ing *i = r->ings; i; prev = i, i = i->next) {
                if (strcmp(i->name, ing_name) == 0) {
                    if (prev) prev->next = i->next;
                    else r->ings = i->next;
                    free(i->name);
                    free(i->note);
                    free(i);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Recipe *prev = NULL;
            for (Recipe *r = recipes; r; prev = r, r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    if (prev) prev->next = r->next;
                    else recipes = r->next;
                    while (r->ings) {
                        Ing *i = r->ings;
                        r->ings = i->next;
                        free(i->name);
                        free(i->note);
                        free(i);
                    }
                    free(r->name);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Recipe *r = find_recipe(name);
            if (r) {
                for (Ing *i = r->ings; i; i = i->next) {
                    printf("%s ", i->name);
                    print_text(i->note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Recipe *r = recipes; r; r = r->next) printf("%s\n", r->name);
        }
    }
    while (recipes) {
        Recipe *r = recipes;
        recipes = recipes->next;
        while (r->ings) {
            Ing *i = r->ings;
            r->ings = i->next;
            free(i->name);
            free(i->note);
            free(i);
        }
        free(r->name);
        free(r);
    }
    return 0;
}