// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *ing; char *note; } Ingredient;
typedef struct { char *name; Ingredient *ings; int nings; } Recipe;
Recipe recipes[1000]; int nrecipes = 0;

int find_recipe(char *name) {
    for (int i = 0; i < nrecipes; i++) if (!strcmp(recipes[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], ing[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "RECIPE")) {
            scanf("%s", name);
            recipes[nrecipes].name = strdup(name);
            recipes[nrecipes].ings = NULL;
            recipes[nrecipes++].nings = 0;
        } else if (!strcmp(cmd, "ING")) {
            scanf("%s", name);
            fgets(ing, sizeof(ing), stdin);
            ing[strcspn(ing, "\n")] = 0;
            if (ing[0] == ' ') memmove(ing, ing + 1, strlen(ing));
            char *space = strchr(ing, ' ');
            if (space) {
                *space = 0;
                char *n = space + 1;
                while(*n == ' ') n++;
                int idx = find_recipe(name);
                if (idx != -1) {
                    recipes[idx].ings = realloc(recipes[idx].ings, sizeof(Ingredient) * (recipes[idx].nings + 1));
                    recipes[idx].ings[recipes[idx].nings].ing = strdup(ing);
                    recipes[idx].ings[recipes[idx].nings++].note = strdup(n);
                }
            }
        } else if (!strcmp(cmd, "REMOVEING")) {
            scanf("%s %s", name, ing);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = 0; i < recipes[idx].nings; i++) {
                    if (!strcmp(recipes[idx].ings[i].ing, ing)) {
                        free(recipes[idx].ings[i].ing);
                        free(recipes[idx].ings[i].note);
                        for (int j = i; j < recipes[idx].nings - 1; j++) recipes[idx].ings[j] = recipes[idx].ings[j + 1];
                        recipes[idx].nings--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETERECIPE")) {
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                free(recipes[idx].name);
                for (int i = 0; i < recipes[idx].nings; i++) {
                    free(recipes[idx].ings[i].ing);
                    free(recipes[idx].ings[i].note);
                }
                free(recipes[idx].ings);
                for (int i = idx; i < nrecipes - 1; i++) recipes[i] = recipes[i + 1];
                nrecipes--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                printf("%s\n", recipes[idx].name);
                for (int i = 0; i < recipes[idx].nings; i++) {
                    printf("%s %s\n", recipes[idx].ings[i].ing, recipes[idx].ings[i].note);
                }
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nrecipes; i++) printf("%s\n", recipes[i].name);
        }
    }
    for (int i = 0; i < nrecipes; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].nings; j++) {
            free(recipes[i].ings[j].ing);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    return 0;
}