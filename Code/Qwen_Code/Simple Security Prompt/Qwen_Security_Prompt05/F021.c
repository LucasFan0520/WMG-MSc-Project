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
    int num_ings;
} Recipe;

Recipe *recipes = NULL;
int num_recipes = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < num_recipes; i++) {
        if (strcmp(recipes[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            recipes = realloc(recipes, (num_recipes + 1) * sizeof(Recipe));
            recipes[num_recipes].name = strdup(name);
            recipes[num_recipes].ings = NULL;
            recipes[num_recipes].num_ings = 0;
            num_recipes++;
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256], ing[256];
            sscanf(line, "%*s %s %s", name, ing);
            char *note = strstr(line, ing);
            if (note) {
                note += strlen(ing);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            int idx = find_recipe(name);
            if (idx >= 0) {
                recipes[idx].ings = realloc(recipes[idx].ings, (recipes[idx].num_ings + 1) * sizeof(Ingredient));
                recipes[idx].ings[recipes[idx].num_ings].ingredient = strdup(ing);
                recipes[idx].ings[recipes[idx].num_ings].note = strdup(note);
                recipes[idx].num_ings++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256], ing[256];
            sscanf(line, "%*s %s %s", name, ing);
            int idx = find_recipe(name);
            if (idx >= 0) {
                for (int i = 0; i < recipes[idx].num_ings; i++) {
                    if (strcmp(recipes[idx].ings[i].ingredient, ing) == 0) {
                        free(recipes[idx].ings[i].ingredient);
                        free(recipes[idx].ings[i].note);
                        for (int j = i; j < recipes[idx].num_ings - 1; j++) {
                            recipes[idx].ings[j] = recipes[idx].ings[j + 1];
                        }
                        recipes[idx].num_ings--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_recipe(name);
            if (idx >= 0) {
                for (int i = 0; i < recipes[idx].num_ings; i++) {
                    free(recipes[idx].ings[i].ingredient);
                    free(recipes[idx].ings[i].note);
                }
                free(recipes[idx].ings);
                free(recipes[idx].name);
                for (int i = idx; i < num_recipes - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                num_recipes--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_recipe(name);
            if (idx >= 0) {
                for (int i = 0; i < recipes[idx].num_ings; i++) {
                    printf("%s %s\n", recipes[idx].ings[i].ingredient, recipes[idx].ings[i].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_recipes; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    
    for (int i = 0; i < num_recipes; i++) {
        for (int j = 0; j < recipes[i].num_ings; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
        free(recipes[i].name);
    }
    free(recipes);
    
    return 0;
}