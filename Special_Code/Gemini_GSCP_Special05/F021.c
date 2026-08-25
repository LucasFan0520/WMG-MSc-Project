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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    size_t count;
    size_t cap;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t r_count = 0;
    size_t r_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            if (r_count >= r_cap) {
                r_cap = r_cap == 0 ? 4 : r_cap * 2;
                Recipe *nb = realloc(recipes, r_cap * sizeof(Recipe));
                if (!nb) {
                    free(line);
                    break;
                }
                recipes = nb;
            }
            recipes[r_count].name = safe_dup(name);
            recipes[r_count].ings = NULL;
            recipes[r_count].count = 0;
            recipes[r_count].cap = 0;
            r_count++;
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    if (r->count >= r->cap) {
                        r->cap = r->cap == 0 ? 4 : r->cap * 2;
                        Ingredient *nb = realloc(r->ings, r->cap * sizeof(Ingredient));
                        if (!nb) break;
                        r->ings = nb;
                    }
                    r->ings[r->count].name = safe_dup(ing_name);
                    r->ings[r->count].note = safe_dup(note);
                    r->count++;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *ing_name = "";
            if (*p == ' ') {
                *p = '\0';
                ing_name = p + 1;
            }
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    for (size_t j = 0; j < r->count; j++) {
                        if (strcmp(r->ings[j].name, ing_name) == 0) {
                            free(r->ings[j].name);
                            free(r->ings[j].note);
                            for (size_t k = j; k < r->count - 1; k++) {
                                r->ings[k] = r->ings[k + 1];
                            }
                            r->count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].count; j++) {
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
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].count; j++) {
                        printf("  %s: %s\n", recipes[i].ings[j].name, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < r_count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].count; j++) {
            free(recipes[i].ings[j].name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
