// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int ing_count;
    int ing_cap;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    int r_count = 0;
    int r_cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (r_count >= r_cap) {
                    r_cap = r_cap == 0 ? 16 : r_cap * 2;
                    Recipe *nb = realloc(recipes, r_cap * sizeof(Recipe));
                    if (nb) recipes = nb;
                }
                recipes[r_count].name = strdup(name);
                recipes[r_count].ings = NULL;
                recipes[r_count].ing_count = 0;
                recipes[r_count].ing_cap = 0;
                r_count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    if (r->ing_count >= r->ing_cap) {
                        r->ing_cap = r->ing_cap == 0 ? 16 : r->ing_cap * 2;
                        Ingredient *nb = realloc(r->ings, r->ing_cap * sizeof(Ingredient));
                        if (nb) r->ings = nb;
                    }
                    r->ings[r->ing_count].ing_name = strdup(ing);
                    r->ings[r->ing_count].note = strdup(note);
                    r->ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    for (int j = 0; j < r->ing_count; j++) {
                        if (strcmp(r->ings[j].ing_name, ing) == 0) {
                            free(r->ings[j].ing_name);
                            free(r->ings[j].note);
                            for (int k = j; k < r->ing_count - 1; k++) {
                                r->ings[k] = r->ings[k + 1];
                            }
                            r->ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ings[j].ing_name);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    for (int j = i; j < r_count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("RECIPE %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("%s %s\n", recipes[i].ings[j].ing_name, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < r_count; i++) {
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
