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
    Ingredient *ing_head;
    struct Recipe *next;
} Recipe;

Recipe *r_head = NULL;

Recipe *find_recipe(const char *name) {
    Recipe *r = r_head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Ingredient *find_ingredient(Recipe *r, const char *name) {
    Ingredient *i = r->ing_head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void create_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    r->name = strdup(name);
    r->ing_head = NULL;
    r->next = r_head;
    r_head = r;
}

void add_ingredient(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    if (find_ingredient(r, iname)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ing_head;
    r->ing_head = i;
}

void remove_ingredient(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ingredient *prev = NULL;
    Ingredient *i = r->ing_head;
    while (i) {
        if (strcmp(i->name, iname) == 0) {
            if (prev) prev->next = i->next;
            else r->ing_head = i->next;
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
    Recipe *prev = NULL;
    Recipe *r = r_head;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else r_head = r->next;
            free(r->name);
            Ingredient *i = r->ing_head;
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
        r = r->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s\n", r->name);
    Ingredient *i = r->ing_head;
    while (i) {
        printf("%s ", i->name);
        for (char *p = i->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        i = i->next;
    }
}

void list_recipes(void) {
    Recipe *r = r_head;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    while (r_head) {
        Recipe *next = r_head->next;
        free(r_head->name);
        Ingredient *i = r_head->ing_head;
        while (i) {
            Ingredient *nexti = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = nexti;
        }
        free(r_head);
        r_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                create_recipe(name);
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char rname[256], iname[256], note[1024];
            if (sscanf(line, "%*s %255s %255s %1023[^\n]", rname, iname, note) == 3) {
                add_ingredient(rname, iname, note);
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char rname[256], iname[256];
            if (sscanf(line, "%*s %255s %255s", rname, iname) == 2) {
                remove_ingredient(rname, iname);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_recipe(name);
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                show_recipe(name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_recipes();
        }
    }

    free(line);
    free_all();
    return 0;
}