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
    int count;
    int cap;
} Recipe;

Recipe *recipes = NULL;
int r_count = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < r_count; i++) {
        if (strcmp(recipes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_ingredient(int ri, const char *ing) {
    for (int i = 0; i < recipes[ri].count; i++) {
        if (strcmp(recipes[ri].ings[i].ingredient, ing) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char name[1024];
    char ing[1024];
    char note[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "RECIPE") == 0) {
            scanf("%s", name);
            if (find_recipe(name) == -1) {
                recipes = realloc(recipes, sizeof(Recipe) * (r_count + 1));
                recipes[r_count].name = strdup(name);
                recipes[r_count].ings = NULL;
                recipes[r_count].count = 0;
                recipes[r_count].cap = 0;
                r_count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            scanf("%s %s", name, ing);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            int ri = find_recipe(name);
            if (ri != -1) {
                if (recipes[ri].count == recipes[ri].cap) {
                    recipes[ri].cap = recipes[ri].cap ? recipes[ri].cap * 2 : 4;
                    recipes[ri].ings = realloc(recipes[ri].ings, sizeof(Ingredient) * recipes[ri].cap);
                }
                recipes[ri].ings[recipes[ri].count].ingredient = strdup(ing);
                recipes[ri].ings[recipes[ri].count].note = strdup(note);
                recipes[ri].count++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            scanf("%s %s", name, ing);
            int ri = find_recipe(name);
            if (ri != -1) {
                int ii = find_ingredient(ri, ing);
                if (ii != -1) {
                    free(recipes[ri].ings[ii].ingredient);
                    free(recipes[ri].ings[ii].note);
                    for (int i = ii; i < recipes[ri].count - 1; i++) {
                        recipes[ri].ings[i] = recipes[ri].ings[i + 1];
                    }
                    recipes[ri].count--;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            scanf("%s", name);
            int ri = find_recipe(name);
            if (ri != -1) {
                free(recipes[ri].name);
                for (int i = 0; i < recipes[ri].count; i++) {
                    free(recipes[ri].ings[i].ingredient);
                    free(recipes[ri].ings[i].note);
                }
                free(recipes[ri].ings);
                for (int i = ri; i < r_count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                r_count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            int ri = find_recipe(name);
            if (ri != -1) {
                printf("%s\n", recipes[ri].name);
                for (int i = 0; i < recipes[ri].count; i++) {
                    printf("%s %s\n", recipes[ri].ings[i].ingredient, recipes[ri].ings[i].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }

    for (int i = 0; i < r_count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].count; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);

    return 0;
}