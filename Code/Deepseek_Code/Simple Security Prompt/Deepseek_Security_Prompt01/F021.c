// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char **ings; int ic; int icap; } Recipe;
Recipe *recipes = NULL;
int rc = 0, rcap = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < rc; i++)
        if (strcmp(recipes[i].name, name) == 0) return i;
    return -1;
}

int find_ingredient(Recipe *r, const char *ing) {
    for (int i = 0; i < r->ic; i++)
        if (strcmp(r->ings[i], ing) == 0) return i;
    return -1;
}

void add_recipe(const char *name) {
    if (find_recipe(name) >= 0) return;
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; recipes = realloc(recipes, sizeof(Recipe) * rcap); }
    recipes[rc].name = strdup(name);
    recipes[rc].ings = NULL;
    recipes[rc].ic = 0;
    recipes[rc].icap = 0;
    rc++;
}

void add_ingredient(const char *name, const char *ing, const char *note) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    Recipe *r = &recipes[idx];
    char *full = malloc(strlen(ing) + strlen(note) + 2);
    sprintf(full, "%s %s", ing, note);
    if (find_ingredient(r, full) >= 0) { free(full); return; }
    if (r->ic == r->icap) { r->icap = r->icap ? r->icap * 2 : 4; r->ings = realloc(r->ings, sizeof(char *) * r->icap); }
    r->ings[r->ic] = full;
    r->ic++;
}

void remove_ingredient(const char *name, const char *ing) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    Recipe *r = &recipes[idx];
    for (int i = 0; i < r->ic; i++) {
        if (strstr(r->ings[i], ing) == r->ings[i] && r->ings[i][strlen(ing)] == ' ') {
            free(r->ings[i]);
            for (int j = i; j < r->ic - 1; j++) r->ings[j] = r->ings[j + 1];
            r->ic--;
            return;
        }
    }
}

void delete_recipe(const char *name) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    for (int i = 0; i < recipes[idx].ic; i++) free(recipes[idx].ings[i]);
    free(recipes[idx].ings);
    free(recipes[idx].name);
    for (int i = idx; i < rc - 1; i++) recipes[i] = recipes[i + 1];
    rc--;
}

void show_recipe(const char *name) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    printf("%s\n", recipes[idx].name);
    for (int i = 0; i < recipes[idx].ic; i++)
        printf("%s\n", recipes[idx].ings[i]);
}

void list_recipes(void) {
    for (int i = 0; i < rc; i++) printf("%s\n", recipes[i].name);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) add_recipe(arg1);
        } else if (strcmp(cmd, "ING") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, line) == 3) {
                add_ingredient(arg1, arg2, line);
            } else {
                char ingredient[256], note[4096];
                if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, ingredient, note) == 3)
                    add_ingredient(arg1, ingredient, note);
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char ing[4096];
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, ing) == 2)
                remove_ingredient(arg1, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_recipe(arg1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) show_recipe(arg1);
        } else if (strcmp(cmd, "LIST") == 0) list_recipes();
    }
    for (int i = 0; i < rc; i++) {
        for (int j = 0; j < recipes[i].ic; j++) free(recipes[i].ings[j]);
        free(recipes[i].ings);
        free(recipes[i].name);
    }
    free(recipes);
    return 0;
}