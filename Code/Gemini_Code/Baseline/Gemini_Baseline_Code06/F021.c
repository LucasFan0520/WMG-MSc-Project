// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int ing_count;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            recipes = realloc(recipes, (count + 1) * sizeof(Recipe));
            recipes[count].name = strdup(name);
            recipes[count].ings = NULL;
            recipes[count].ing_count = 0;
            count++;
        } else if (strncmp(line, "ING ", 4) == 0) {
            char name[128], ing[128];
            if (sscanf(line + 4, "%127s %127s", name, ing) == 2) {
                char *p = line + 4;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        recipes[i].ings = realloc(recipes[i].ings, (recipes[i].ing_count + 1) * sizeof(Ingredient));
                        recipes[i].ings[recipes[i].ing_count].ingredient = strdup(ing);
                        recipes[i].ings[recipes[i].ing_count].note = strdup(p);
                        recipes[i].ing_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char name[128], ing[128];
            if (sscanf(line + 10, "%127s %127s", name, ing) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ings[j].ingredient, ing) == 0) {
                                free(recipes[i].ings[j].ingredient);
                                free(recipes[i].ings[j].note);
                                for (int m = j; m < recipes[i].ing_count - 1; m++) {
                                    recipes[i].ings[m] = recipes[i].ings[m + 1];
                                }
                                recipes[i].ing_count--;
                                recipes[i].ings = realloc(recipes[i].ings, recipes[i].ing_count * sizeof(Ingredient));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ings[j].ingredient);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    for (int j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    recipes = realloc(recipes, count * sizeof(Recipe));
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("  %s: %s\n", recipes[i].ings[j].ingredient, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
