// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECIPES 1000
#define MAX_ING 1000
#define MAX_LEN 1000

typedef struct {
    char ingredient[100];
    char note[MAX_LEN];
} Ingredient;

typedef struct {
    char name[100];
    Ingredient ings[MAX_ING];
    int icount;
} Recipe;

Recipe recipes[MAX_RECIPES];
int rcount = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < rcount; i++) {
        if (strcmp(recipes[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], ing[100], note[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "RECIPE") == 0) {
            scanf("%s", name);
            if (find_recipe(name) == -1 && rcount < MAX_RECIPES) {
                strcpy(recipes[rcount].name, name);
                recipes[rcount].icount = 0;
                rcount++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            scanf("%s %s", name, ing);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find_recipe(name);
            if (idx != -1 && recipes[idx].icount < MAX_ING) {
                strcpy(recipes[idx].ings[recipes[idx].icount].ingredient, ing);
                strcpy(recipes[idx].ings[recipes[idx].icount].note, note);
                recipes[idx].icount++;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            scanf("%s %s", name, ing);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = 0; i < recipes[idx].icount; i++) {
                    if (strcmp(recipes[idx].ings[i].ingredient, ing) == 0) {
                        for (int j = i; j < recipes[idx].icount - 1; j++) {
                            recipes[idx].ings[j] = recipes[idx].ings[j + 1];
                        }
                        recipes[idx].icount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                for (int i = idx; i < rcount - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                rcount--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            int idx = find_recipe(name);
            if (idx != -1) {
                printf("%s\n", recipes[idx].name);
                for (int i = 0; i < recipes[idx].icount; i++) {
                    printf("%s ", recipes[idx].ings[i].ingredient);
                    for (char *p = recipes[idx].ings[i].note; *p; p++) {
                        putchar(*p == ' ' ? '_' : *p);
                    }
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    return 0;
}