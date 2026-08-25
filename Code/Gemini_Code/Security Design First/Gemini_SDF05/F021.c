// F021.c
#define _GNU_SOURCE
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
    int ing_count;
    int ing_capacity;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Recipe *tmp = realloc(recipes, new_cap * sizeof(Recipe));
                if (!tmp) return 1;
                recipes = tmp;
                capacity = new_cap;
            }
            recipes[count].name = strdup(name);
            recipes[count].ingredients = NULL;
            recipes[count].ing_count = 0;
            recipes[count].ing_capacity = 0;
            count++;
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *ing = space1 + 1;
            while (*ing == ' ') ing++;
            char *space2 = strchr(ing, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            int r_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    r_idx = i;
                    break;
                }
            }
            if (r_idx != -1) {
                Recipe *r = &recipes[r_idx];
                if (r->ing_count >= r->ing_capacity) {
                    int new_icap = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                    Ingredient *tmp = realloc(r->ingredients, new_icap * sizeof(Ingredient));
                    if (!tmp) return 1;
                    r->ingredients = tmp;
                    r->ing_capacity = new_icap;
                }
                r->ingredients[r->ing_count].ingredient = strdup(ing);
                r->ingredients[r->ing_count].note = strdup(note);
                r->ing_count++;
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *ing = space + 1;
            while (*ing == ' ') ing++;
            int r_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    r_idx = i;
                    break;
                }
            }
            if (r_idx != -1) {
                Recipe *r = &recipes[r_idx];
                int i_idx = -1;
                for (int j = 0; j < r->ing_count; j++) {
                    if (strcmp(r->ingredients[j].ingredient, ing) == 0) {
                        i_idx = j;
                        break;
                    }
                }
                if (i_idx != -1) {
                    free(r->ingredients[i_idx].ingredient);
                    free(r->ingredients[i_idx].note);
                    for (int j = i_idx; j < r->ing_count - 1; j++) {
                        r->ingredients[j] = r->ingredients[j + 1];
                    }
                    r->ing_count--;
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            while (*name == ' ') name++;
            int r_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    r_idx = i;
                    break;
                }
            }
            if (r_idx != -1) {
                free(recipes[r_idx].name);
                for (int j = 0; j < recipes[r_idx].ing_count; j++) {
                    free(recipes[r_idx].ingredients[j].ingredient);
                    free(recipes[r_idx].ingredients[j].note);
                }
                free(recipes[r_idx].ingredients);
                for (int i = r_idx; i < count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int r_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    r_idx = i;
                    break;
                }
            }
            if (r_idx != -1) {
                printf("Recipe: %s\n", recipes[r_idx].name);
                for (int j = 0; j < recipes[r_idx].ing_count; j++) {
                    printf("%s %s\n", recipes[r_idx].ingredients[j].ingredient, recipes[r_idx].ingredients[j].note);
                }
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].ingredient);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
