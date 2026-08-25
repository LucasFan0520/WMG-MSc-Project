// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
} Recipe;

typedef struct {
    char *recipe_name;
    char *ing_name;
    char *note;
} Ingredient;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Recipe *recipes = NULL;
    int r_count = 0;
    Ingredient *ings = NULL;
    int i_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < r_count; i++) {
                    printf("%s\n", recipes[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "RECIPE") == 0) {
            int found = -1;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, rest) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                recipes = realloc(recipes, (r_count + 1) * sizeof(Recipe));
                recipes[r_count].name = strdup(rest);
                r_count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *rname = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *iname = rest2;
                    char *note = p3 + 1;
                    int r_found = -1;
                    for (int i = 0; i < r_count; i++) {
                        if (strcmp(recipes[i].name, rname) == 0) {
                            r_found = i;
                            break;
                        }
                    }
                    if (r_found != -1) {
                        ings = realloc(ings, (i_count + 1) * sizeof(Ingredient));
                        ings[i_count].recipe_name = strdup(rname);
                        ings[i_count].ing_name = strdup(iname);
                        ings[i_count].note = strdup(note);
                        i_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *rname = rest;
                char *iname = p2 + 1;
                int found = -1;
                for (int i = 0; i < i_count; i++) {
                    if (strcmp(ings[i].recipe_name, rname) == 0 && strcmp(ings[i].ing_name, inname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(ings[found].recipe_name);
                    free(ings[found].ing_name);
                    free(ings[found].note);
                    for (int i = found; i < i_count - 1; i++) {
                        ings[i] = ings[i + 1];
                    }
                    i_count--;
                    if (i_count == 0) {
                        free(ings);
                        ings = NULL;
                    } else {
                        ings = realloc(ings, i_count * sizeof(Ingredient));
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *rname = rest;
            int r_found = -1;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    r_found = i;
                    break;
                }
            }
            if (r_found != -1) {
                free(recipes[r_found].name);
                for (int i = r_found; i < r_count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                r_count--;
                if (r_count == 0) {
                    free(recipes);
                    recipes = NULL;
                } else {
                    recipes = realloc(recipes, r_count * sizeof(Recipe));
                }
                int idx = 0;
                while (idx < i_count) {
                    if (strcmp(ings[idx].recipe_name, rname) == 0) {
                        free(ings[idx].recipe_name);
                        free(ings[idx].ing_name);
                        free(ings[idx].note);
                        for (int j = idx; j < i_count - 1; j++) {
                            ings[j] = ings[j + 1];
                        }
                        i_count--;
                    } else {
                        idx++;
                    }
                }
                if (i_count == 0) {
                    free(ings);
                    ings = NULL;
                } else {
                    ings = realloc(ings, i_count * sizeof(Ingredient));
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *rname = rest;
            int r_found = -1;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    r_found = i;
                    break;
                }
            }
            if (r_found != -1) {
                printf("Recipe: %s\n", rname);
                for (int i = 0; i < i_count; i++) {
                    if (strcmp(ings[i].recipe_name, rname) == 0) {
                        printf("%s: %s\n", ings[i].ing_name, ings[i].note);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < r_count; i++) {
        free(recipes[i].name);
    }
    free(recipes);
    for (int i = 0; i < i_count; i++) {
        free(ings[i].recipe_name);
        free(ings[i].ing_name);
        free(ings[i].note);
    }
    free(ings);
    return 0;
}