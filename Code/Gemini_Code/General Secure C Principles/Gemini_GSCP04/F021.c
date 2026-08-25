// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    size_t ing_count;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Recipe *new_recipes = realloc(recipes, capacity * sizeof(Recipe));
                    if (new_recipes) recipes = new_recipes;
                }
                if (count < capacity) {
                    recipes[count].name = strdup(name);
                    recipes[count].ingredients = NULL;
                    recipes[count].ing_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *rname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *ing_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    recipes[i].ingredients = realloc(recipes[i].ingredients, (recipes[i].ing_count + 1) * sizeof(Ingredient));
                    recipes[i].ingredients[recipes[i].ing_count].name = strdup(ing_name);
                    recipes[i].ingredients[recipes[i].ing_count].note = strdup(note);
                    recipes[i].ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *rname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *ing_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        if (strcmp(recipes[i].ingredients[k].name, ing_name) == 0) {
                            free(recipes[i].ingredients[k].name);
                            free(recipes[i].ingredients[k].note);
                            for (size_t j = k; j < recipes[i].ing_count - 1; j++) {
                                recipes[i].ingredients[j] = recipes[i].ingredients[j + 1];
                            }
                            recipes[i].ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        free(recipes[i].ingredients[k].name);
                        free(recipes[i].ingredients[k].note);
                    }
                    free(recipes[i].ingredients);
                    for (size_t j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("%s\n", recipes[i].name);
                    for (size_t k = 0; k < recipes[i].ing_count; k++) {
                        printf("%s %s\n", recipes[i].ingredients[k].name, recipes[i].ingredients[k].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(recipes[i].name);
        for (size_t k = 0; k < recipes[i].ing_count; k++) {
            free(recipes[i].ingredients[k].name);
            free(recipes[i].ingredients[k].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
