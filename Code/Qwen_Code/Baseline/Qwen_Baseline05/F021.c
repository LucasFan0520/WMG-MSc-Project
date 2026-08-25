// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_ING 100
#define MAX_NOTE 1000

typedef struct {
    char ingredient[MAX_ING];
    char note[MAX_NOTE];
} Ingredient;

typedef struct {
    char name[MAX_NAME];
    Ingredient ings[1000];
    int ing_count;
} Recipe;

Recipe recipes[1000];
int recipe_count = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < recipe_count; i++) {
        if (strcmp(recipes[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], ing[MAX_ING], note[MAX_NOTE];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "RECIPE") == 0) {
            scanf("%s", name);
            strcpy(recipes[recipe_count].name, name);
            recipes[recipe_count].ing_count = 0;
            recipe_count++;
        } else if (strcmp(cmd, "ING") == 0) {
            scanf("%s %s", name, ing);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            int idx = find_recipe(name);
            if (idx != -1) {
                strcpy(recipes[idx].ings[recipes[idx].ing_count].ingredient, ing);
                strcpy(recipes[idx].ings[recipes[idx].ing_count].note, note);
                recipes[idx].ing_count++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            scanf("%s %s", name, ing);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = 0; i < recipes[idx].ing_count; i++) {
                    if (strcmp(recipes[idx].ings[i].ingredient, ing) == 0) {
                        for (int j = i; j < recipes[idx].ing_count - 1; j++) {
                            recipes[idx].ings[j] = recipes[idx].ings[j + 1];
                        }
                        recipes[idx].ing_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = idx; i < recipe_count - 1; i++) recipes[i] = recipes[i + 1];
                recipe_count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                printf("%s\n", recipes[idx].name);
                for (int i = 0; i < recipes[idx].ing_count; i++) {
                    printf("  %s %s\n", recipes[idx].ings[i].ingredient, recipes[idx].ings[i].note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < recipe_count; i++) printf("%s\n", recipes[i].name);
        }
    }
    return 0;
}