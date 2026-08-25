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
    int ic;
} Recipe;

Recipe *recipes = NULL;
int rc = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < rc; i++) {
        if (strcmp(recipes[i].name, name) == 0) return i;
    }
    return -1;
}

int find_ingredient(int ri, const char *ing) {
    for (int j = 0; j < recipes[ri].ic; j++) {
        if (strcmp(recipes[ri].ings[j].name, ing) == 0) return j;
    }
    return -1;
}

int main(void) {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256];
        char arg2[256];
        char rest[1024];
        arg1[0] = '\0';
        arg2[0] = '\0';
        rest[0] = '\0';
        int parsed = sscanf(line, "%31s %255s %255s %1023[^\n]", cmd, arg1, arg2, rest);
        if (parsed < 1) continue;

        if (strcmp(cmd, "RECIPE") == 0 && parsed >= 2) {
            if (find_recipe(arg1) < 0) {
                recipes = realloc(recipes, (rc + 1) * sizeof(Recipe));
                if (!recipes) return 1;
                recipes[rc].name = strdup(arg1);
                recipes[rc].ings = NULL;
                recipes[rc].ic = 0;
                rc++;
            }
        } else if (strcmp(cmd, "ING") == 0 && parsed >= 3) {
            int ri = find_recipe(arg1);
            if (ri >= 0) {
                recipes[ri].ings = realloc(recipes[ri].ings, (recipes[ri].ic + 1) * sizeof(Ingredient));
                if (!recipes[ri].ings) return 1;
                recipes[ri].ings[recipes[ri].ic].name = strdup(arg2);
                recipes[ri].ings[recipes[ri].ic].note = strdup(rest);
                recipes[ri].ic++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0 && parsed >= 3) {
            int ri = find_recipe(arg1);
            if (ri >= 0) {
                int ii = find_ingredient(ri, arg2);
                if (ii >= 0) {
                    free(recipes[ri].ings[ii].name);
                    free(recipes[ri].ings[ii].note);
                    for (int k = ii; k < recipes[ri].ic - 1; k++) {
                        recipes[ri].ings[k] = recipes[ri].ings[k + 1];
                    }
                    recipes[ri].ic--;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0 && parsed >= 2) {
            int ri = find_recipe(arg1);
            if (ri >= 0) {
                for (int j = 0; j < recipes[ri].ic; j++) {
                    free(recipes[ri].ings[j].name);
                    free(recipes[ri].ings[j].note);
                }
                free(recipes[ri].ings);
                free(recipes[ri].name);
                for (int k = ri; k < rc - 1; k++) {
                    recipes[k] = recipes[k + 1];
                }
                rc--;
            }
        } else if (strcmp(cmd, "SHOW") == 0 && parsed >= 2) {
            int ri = find_recipe(arg1);
            if (ri >= 0) {
                printf("%s\n", recipes[ri].name);
                for (int j = 0; j < recipes[ri].ic; j++) {
                    printf("%s %s\n", recipes[ri].ings[j].name, recipes[ri].ings[j].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        for (int j = 0; j < recipes[i].ic; j++) {
            free(recipes[i].ings[j].name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
        free(recipes[i].name);
    }
    free(recipes);
    return 0;
}