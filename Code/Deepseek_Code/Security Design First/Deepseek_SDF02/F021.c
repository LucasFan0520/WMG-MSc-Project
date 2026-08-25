/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ing {
    char *name;
    char *note;
    struct ing *next;
} ing_t;

typedef struct recipe {
    char *name;
    ing_t *ings;
    struct recipe *next;
} recipe_t;

recipe_t *recipes = NULL;

recipe_t *find_recipe(const char *name) {
    recipe_t *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void recipe_cmd(const char *name) {
    if (find_recipe(name)) return;
    recipe_t *r = malloc(sizeof(recipe_t));
    if (!r) return;
    r->name = strdup(name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
    if (!r->name) { free(r); }
}

void ing_cmd(const char *name, const char *ing, const char *note) {
    recipe_t *r = find_recipe(name);
    if (!r) return;
    ing_t *i = malloc(sizeof(ing_t));
    if (!i) return;
    i->name = strdup(ing);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
    if (!i->name || !i->note) {
        free(i->name); free(i->note); free(i);
    }
}

void removeing_cmd(const char *name, const char *ing) {
    recipe_t *r = find_recipe(name);
    if (!r) return;
    ing_t *prev = NULL, *i = r->ings;
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

void deleterecipe_cmd(const char *name) {
    recipe_t *prev = NULL, *r = recipes;
    while (r) {
        if (strcmp(r->name, name) == 0) {
            if (prev) prev->next = r->next;
            else recipes = r->next;
            free(r->name);
            ing_t *i = r->ings;
            while (i) {
                ing_t *nxt = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = nxt;
            }
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void show_cmd(const char *name) {
    recipe_t *r = find_recipe(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->name);
    ing_t *i = r->ings;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void list_cmd(void) {
    recipe_t *r = recipes;
    while (r) {
        printf("%s\n", r->name);
        r = r->next;
    }
}

void free_all(void) {
    recipe_t *r = recipes;
    while (r) {
        recipe_t *nxt = r->next;
        free(r->name);
        ing_t *i = r->ings;
        while (i) {
            ing_t *n = i->next;
            free(i->name);
            free(i->note);
            free(i);
            i = n;
        }
        free(r);
        r = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[256], b[256];
        if (sscanf(line, "%19s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "RECIPE") == 0) {
                recipe_cmd(a);
            } else if (strcmp(cmd, "ING") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2) {
                    const char *note = line + strlen(cmd) + 1 + strlen(a) + 1 + strlen(b) + 1;
                    ing_cmd(a, b, note);
                }
            } else if (strcmp(cmd, "REMOVEING") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    removeing_cmd(a, b);
            } else if (strcmp(cmd, "DELETERECIPE") == 0) {
                deleterecipe_cmd(a);
            } else if (strcmp(cmd, "SHOW") == 0) {
                show_cmd(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}