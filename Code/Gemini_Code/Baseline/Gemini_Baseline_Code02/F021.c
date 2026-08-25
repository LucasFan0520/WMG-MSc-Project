// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *ing_name;
    char *note;
} Ing;

typedef struct {
    char *name;
    Ing *ings;
    int ing_count;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", recipes[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "RECIPE") == 0) {
            recipes = realloc(recipes, sizeof(Recipe) * (count + 1));
            recipes[count].name = strdup(args);
            recipes[count].ings = NULL;
            recipes[count].ing_count = 0;
            count++;
        } else if (strcmp(cmd, "ING") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *rname = args;
                char *ing_name = p2 + 1;
                char *p3 = strchr(ing_name, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(recipes[i].name, rname) == 0) {
                            int ic = recipes[i].ing_count;
                            recipes[i].ings = realloc(recipes[i].ings, sizeof(Ing) * (ic + 1));
                            recipes[i].ings[ic].ing_name = strdup(ing_name);
                            recipes[i].ings[ic].note = strdup(note);
                            recipes[i].ing_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *rname = args;
                char *ing_name = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ings[j].ing_name, ing_name) == 0) {
                                free(recipes[i].ings[j].ing_name);
                                free(recipes[i].ings[j].note);
                                for (int k = j; k < recipes[i].ing_count - 1; k++) {
                                    recipes[i].ings[k] = recipes[i].ings[k + 1];
                                }
                                recipes[i].ing_count--;
                                if (recipes[i].ing_count == 0) {
                                    free(recipes[i].ings);
                                    recipes[i].ings = NULL;
                                } else {
                                    recipes[i].ings = realloc(recipes[i].ings, sizeof(Ing) * recipes[i].ing_count);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, args) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ings[j].ing_name);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    for (int j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    if (count == 0) { free(recipes); recipes = NULL; }
                    else { recipes = realloc(recipes, sizeof(Recipe) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, args) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("- %s: %s\n", recipes[i].ings[j].ing_name, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].ing_name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
