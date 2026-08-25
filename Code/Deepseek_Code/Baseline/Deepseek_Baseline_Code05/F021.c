/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ingredient {
    char *name;
    char *note;
    struct ingredient *next;
} Ingredient;

typedef struct recipe {
    char *name;
    Ingredient *ings;
    struct recipe *next;
} Recipe;

Recipe *recipes = NULL;

Recipe *find_recipe(const char *name) {
    Recipe *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void create_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ingredient(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *i = r->ings;
    while (i) {
        if (strcmp(i->name, iname) == 0) {
            free(i->note);
            i->note = strdup(note);
            return;
        }
        i = i->next;
    }
    Ingredient *new = malloc(sizeof(Ingredient));
    new->name = strdup(iname);
    new->note = strdup(note);
    new->next = r->ings;
    r->ings = new;
}

void remove_ingredient(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *i = r->ings, *prev = NULL;
    while (i) {
        if (strcmp(i->name, iname) == 0) {
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

void delete_recipe(const char *name) {
    Recipe *r = recipes, *prev = NULL;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            free(r->name);
            Ingredient *i = r->ings;
            while (i) {
                Ingredient *tmp = i;
                i = i->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->name);
    Ingredient *i = r->ings;
    while (i) {
        printf("%s ", i->name);
        for (char *c = i->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        i = i->next;
    }
}

void list_recipes() {
    Recipe *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], iname[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_recipes(); continue; }
        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) create_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, iname) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1 + strlen(iname) + 1;
            if (*rest == ' ') rest++;
            add_ingredient(name, iname, rest);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, iname) == 2)
                remove_ingredient(name, iname);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_recipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) show_recipe(name);
        }
    }
    return 0;
}