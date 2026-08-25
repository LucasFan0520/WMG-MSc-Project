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
    Ingredient *ingredients;
    int ing_count;
} Recipe;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Recipe *recipes = NULL;
    int recipe_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            recipes = realloc(recipes, (recipe_count + 1) * sizeof(Recipe));
            recipes[recipe_count].name = strdup(name);
            recipes[recipe_count].ingredients = NULL;
            recipes[recipe_count].ing_count = 0;
            recipe_count++;
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ') p++;
            char *rname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            for (int i = 0; i < recipe_count; i++) {
                if (recipes[i].name && strcmp(recipes[i].name, rname) == 0) {
                    int ic = recipes[i].ing_count;
                    recipes[i].ingredients = realloc(recipes[i].ingredients, (ic + 1) * sizeof(Ingredient));
                    recipes[i].ingredients[ic].name = strdup(ing_name);
                    recipes[i].ingredients[ic].note = strdup(note);
                    recipes[i].ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ') p++;
            char *rname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < recipe_count; i++) {
                if (recipes[i].name && strcmp(recipes[i].name, rname) == 0) {
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        if (strcmp(recipes[i].ingredients[j].name, ing_name) == 0) {
                            free(recipes[i].ingredients[j].name);
                            free(recipes[i].ingredients[j].note);
                            for (int k = j; k < recipes[i].ing_count - 1; k++) {
                                recipes[i].ingredients[k] = recipes[i].ingredients[k + 1];
                            }
                            recipes[i].ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p == ' ') p++;
            char *rname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < recipe_count; i++) {
                if (recipes[i].name && strcmp(recipes[i].name, rname) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ingredients[j].name);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    recipes[i].name = NULL;
                    recipes[i].ingredients = NULL;
                    recipes[i].ing_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ') p++;
            char *rname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < recipe_count; i++) {
                if (recipes[i].name && strcmp(recipes[i].name, rname) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("  %s: %s\n", recipes[i].ingredients[j].name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < recipe_count; i++) {
                if (recipes[i].name) {
                    printf("%s\n", recipes[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < recipe_count; i++) {
        if (recipes[i].name) {
            free(recipes[i].name);
            for (int j = 0; j < recipes[i].ing_count; j++) {
                free(recipes[i].ingredients[j].name);
                free(recipes[i].ingredients[j].note);
            }
            free(recipes[i].ingredients);
        }
    }
    free(recipes);
    return 0;
}
