// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int ic;
} Recipe;

Recipe recipes[1000];
int rc = 0;

int find_recipe(char *name) {
    for (int i = 0; i < rc; i++) if (strcmp(recipes[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "RECIPE") == 0) {
            if (find_recipe(rest) == -1) {
                recipes[rc].name = strdup(rest);
                recipes[rc].ings = NULL;
                recipes[rc].ic = 0;
                rc++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *p = rest + strlen(name);
            while (*p == ' ') p++;
            char ing[1000];
            sscanf(p, "%s", ing);
            char *note = p + strlen(ing);
            while (*note == ' ') note++;
            int idx = find_recipe(name);
            if (idx != -1) {
                recipes[idx].ings = realloc(recipes[idx].ings, sizeof(Ingredient) * (recipes[idx].ic + 1));
                recipes[idx].ings[recipes[idx].ic].ingredient = strdup(ing);
                recipes[idx].ings[recipes[idx].ic].note = strdup(note);
                recipes[idx].ic++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[1000], ing[1000];
            sscanf(rest, "%s %s", name, ing);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = 0; i < recipes[idx].ic; i++) {
                    if (strcmp(recipes[idx].ings[i].ingredient, ing) == 0) {
                        free(recipes[idx].ings[i].ingredient);
                        free(recipes[idx].ings[i].note);
                        for (int j = i; j < recipes[idx].ic - 1; j++) recipes[idx].ings[j] = recipes[idx].ings[j+1];
                        recipes[idx].ic--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            int idx = find_recipe(rest);
            if (idx != -1) {
                free(recipes[idx].name);
                for (int i = 0; i < recipes[idx].ic; i++) {
                    free(recipes[idx].ings[i].ingredient);
                    free(recipes[idx].ings[i].note);
                }
                free(recipes[idx].ings);
                for (int i = idx; i < rc - 1; i++) recipes[i] = recipes[i+1];
                rc--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int idx = find_recipe(rest);
            if (idx != -1) {
                printf("%s\n", recipes[idx].name);
                for (int i = 0; i < recipes[idx].ic; i++) {
                    printf("%s %s\n", recipes[idx].ings[i].ingredient, recipes[idx].ings[i].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rc; i++) printf("%s\n", recipes[i].name);
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ic; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    return 0;
}