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
    Ingredient *ingredients;
    struct Recipe *next;
} Recipe;

Recipe *head = NULL;

Recipe *find_recipe(const char *name) {
    Recipe *r = head;
    while (r) {
        if (strcmp(r->name, name) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Ingredient *find_ingredient(Recipe *r, const char *name) {
    Ingredient *i = r->ingredients;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void recipe_create(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    if (!r) return;
    r->name = strdup(name);
    r->ingredients = NULL;
    r->next = head;
    head = r;
}

void ingredient_add(const char *recipe, const char *ing, const char *note) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    if (find_ingredient(r, ing)) return;
    Ingredient *i = malloc(sizeof(Ingredient));
    if (!i) return;
    i->name = strdup(ing);
    i->note = strdup(note);
    i->next = r->ingredients;
    r->ingredients = i;
}

void ingredient_remove(const char *recipe, const char *ing) {
    Recipe *r = find_recipe(recipe);
    if (!r) return;
    Ingredient *prev = NULL, *curr = r->ingredients;
    while (curr) {
        if (strcmp(curr->name, ing) == 0) {
            if (prev) prev->next = curr->next;
            else r->ingredients = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void recipe_delete(const char *name) {
    Recipe *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Ingredient *i = curr->ingredients;
            while (i) {
                Ingredient *tmp = i;
                i = i->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void recipe_show(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->name);
    Ingredient *i = r->ingredients;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        i = i->next;
    }
}

void recipe_list(void) {
    Recipe *r = head;
    int first = 1;
    while (r) {
        if (!first) printf(" ");
        printf("%s", r->name);
        first = 0;
        r = r->next;
    }
    printf("\n");
}

void free_all(void) {
    Recipe *r = head;
    while (r) {
        Recipe *tmp = r;
        r = r->next;
        free(tmp->name);
        Ingredient *i = tmp->ingredients;
        while (i) {
            Ingredient *tmpi = i;
            i = i->next;
            free(tmpi->name);
            free(tmpi->note);
            free(tmpi);
        }
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            recipe_create(p);
        } else if (strcmp(cmd, "ING") == 0) {
            char recipe[256], ing[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", recipe, ing) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    ingredient_add(recipe, ing, note);
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char recipe[256], ing[256];
            if (sscanf(line + 10, "%255s %255s", recipe, ing) == 2) {
                ingredient_remove(recipe, ing);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            recipe_delete(p);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            recipe_show(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            recipe_list();
        }
    }
    free_all();
    return 0;
}