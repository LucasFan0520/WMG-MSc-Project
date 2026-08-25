// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int icount;
} Recipe;

Recipe *recipes = NULL;
int rcount = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < rcount; i++) {
        if (strcmp(recipes[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[100];
            scanf("%s", name);
            if (find_recipe(name) == -1) {
                recipes = realloc(recipes, sizeof(Recipe) * (rcount + 1));
                recipes[rcount].name = strdup(name);
                recipes[rcount].ings = NULL;
                recipes[rcount].icount = 0;
                rcount++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char name[100], ing[100], note[1000];
            scanf("%s %s", name, ing);
            scanf(" %[^\n]", note);
            int idx = find_recipe(name);
            if (idx != -1) {
                recipes[idx].ings = realloc(recipes[idx].ings, sizeof(Ingredient) * (recipes[idx].icount + 1));
                recipes[idx].ings[recipes[idx].icount].name = strdup(ing);
                recipes[idx].ings[recipes[idx].icount].note = strdup(note);
                recipes[idx].icount++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[100], ing[100];
            scanf("%s %s", name, ing);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = 0; i < recipes[idx].icount; i++) {
                    if (strcmp(recipes[idx].ings[i].name, ing) == 0) {
                        free(recipes[idx].ings[i].name);
                        free(recipes[idx].ings[i].note);
                        for (int j = i; j < recipes[idx].icount - 1; j++) {
                            recipes[idx].ings[j] = recipes[idx].ings[j+1];
                        }
                        recipes[idx].icount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                free(recipes[idx].name);
                for (int i = 0; i < recipes[idx].icount; i++) {
                    free(recipes[idx].ings[i].name);
                    free(recipes[idx].ings[i].note);
                }
                free(recipes[idx].ings);
                for (int i = idx; i < rcount - 1; i++) recipes[i] = recipes[i+1];
                rcount--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                printf("%s\n", recipes[idx].name);
                for (int i = 0; i < recipes[idx].icount; i++) {
                    printf("%s %s\n", recipes[idx].ings[i].name, recipes[idx].ings[i].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].icount; j++) {
            free(recipes[i].ings[j].name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}