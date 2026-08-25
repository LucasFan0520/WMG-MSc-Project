// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    size_t ing_count;
    size_t ing_cap;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    size_t r_count = 0;
    size_t r_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            if (r_count >= r_cap) {
                r_cap = r_cap == 0 ? 4 : r_cap * 2;
                recipes = realloc(recipes, r_cap * sizeof(Recipe));
            }
            recipes[r_count].name = mystrdup(name);
            recipes[r_count].ings = NULL;
            recipes[r_count].ing_count = 0;
            recipes[r_count].ing_cap = 0;
            r_count++;
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    if (recipes[i].ing_count >= recipes[i].ing_cap) {
                        recipes[i].ing_cap = recipes[i].ing_cap == 0 ? 4 : recipes[i].ing_cap * 2;
                        recipes[i].ings = realloc(recipes[i].ings, recipes[i].ing_cap * sizeof(Ingredient));
                    }
                    recipes[i].ings[recipes[i].ing_count].name = mystrdup(ing_name);
                    recipes[i].ings[recipes[i].ing_count].note = mystrdup(note);
                    recipes[i].ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        if (strcmp(recipes[i].ings[j].name, ing_name) == 0) {
                            free(recipes[i].ings[j].name);
                            free(recipes[i].ings[j].note);
                            for (size_t k = j; k < recipes[i].ing_count - 1; k++) {
                                recipes[i].ings[k] = recipes[i].ings[k + 1];
                            }
                            recipes[i].ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ings[j].name);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    for (size_t j = i; j < r_count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        printf("  %s: %s\n", recipes[i].ings[j].name, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < r_count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
