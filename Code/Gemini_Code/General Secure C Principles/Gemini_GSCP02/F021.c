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
    Ingredient *ingredients;
    int icount;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    int rcount = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = 0;
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Recipe *tmp = realloc(recipes, (rcount + 1) * sizeof(Recipe));
                if (tmp) {
                    recipes = tmp;
                    recipes[rcount].name = strdup(name);
                    recipes[rcount].ingredients = NULL;
                    recipes[rcount].icount = 0;
                    rcount++;
                }
            }
        } else if (strncmp(line, "ING ", 4) == 0) {
            char rname[256], ing[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", rname, ing) == 2) {
                char *note = strchr(p, ' ');
                if (note) {
                    note = strchr(note + 1, ' ');
                    if (note) {
                        while (*note == ' ') note++;
                    }
                }
                if (!note) note = "";
                for (int i = 0; i < rcount; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        Ingredient *itmp = realloc(recipes[i].ingredients, (recipes[i].icount + 1) * sizeof(Ingredient));
                        if (itmp) {
                            recipes[i].ingredients = itmp;
                            recipes[i].ingredients[recipes[i].icount].ingredient = strdup(ing);
                            recipes[i].ingredients[recipes[i].icount].note = strdup(note);
                            recipes[i].icount++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char rname[256], ing[256];
            if (sscanf(line + 10, "%255s %255s", rname, ing) == 2) {
                for (int i = 0; i < rcount; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        for (int j = 0; j < recipes[i].icount; j++) {
                            if (strcmp(recipes[i].ingredients[j].ingredient, ing) == 0) {
                                free(recipes[i].ingredients[j].ingredient);
                                free(recipes[i].ingredients[j].note);
                                for (int k = j; k < recipes[i].icount - 1; k++) {
                                    recipes[i].ingredients[k] = recipes[i].ingredients[k + 1];
                                }
                                recipes[i].icount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            while (*name == ' ') name++;
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].icount; j++) {
                        free(recipes[i].ingredients[j].ingredient);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    for (int j = i; j < rcount - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    rcount--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].icount; j++) {
                        printf("  %s: %s\n", recipes[i].ingredients[j].ingredient, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < rcount; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].icount; j++) {
            free(recipes[i].ingredients[j].ingredient);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
