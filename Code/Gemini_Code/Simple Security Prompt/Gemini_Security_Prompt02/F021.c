// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
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
    if (len == 0 && c == EOF) {
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
    Ingredient *ingredients;
    size_t icount;
    size_t icap;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    size_t rcount = 0;
    size_t rcap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            if (rcount >= rcap) {
                rcap = rcap == 0 ? 4 : rcap * 2;
                recipes = realloc(recipes, rcap * sizeof(Recipe));
            }
            recipes[rcount].name = strdup(name);
            recipes[rcount].ingredients = NULL;
            recipes[rcount].icount = 0;
            recipes[rcount].icap = 0;
            rcount++;
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *rec_name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                char *ing_name = p2;
                char *note = "";
                if (s2) {
                    *s2 = '\0';
                    note = s2 + 1;
                }
                for (size_t i = 0; i < rcount; i++) {
                    if (strcmp(recipes[i].name, rec_name) == 0) {
                        Recipe *r = &recipes[i];
                        if (r->icount >= r->icap) {
                            r->icap = r->icap == 0 ? 4 : r->icap * 2;
                            r->ingredients = realloc(r->ingredients, r->icap * sizeof(Ingredient));
                        }
                        r->ingredients[r->icount].ing_name = strdup(ing_name);
                        r->ingredients[r->icount].note = strdup(note);
                        r->icount++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *rec_name = p;
                char *ing_name = s1 + 1;
                for (size_t i = 0; i < rcount; i++) {
                    if (strcmp(recipes[i].name, rec_name) == 0) {
                        Recipe *r = &recipes[i];
                        for (size_t j = 0; j < r->icount; j++) {
                            if (strcmp(r->ingredients[j].ing_name, ing_name) == 0) {
                                free(r->ingredients[j].ing_name);
                                free(r->ingredients[j].note);
                                for (size_t k = j; k < r->icount - 1; k++) {
                                    r->ingredients[k] = r->ingredients[k + 1];
                                }
                                r->icount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *rec_name = line + 13;
            for (size_t i = 0; i < rcount; i++) {
                if (strcmp(recipes[i].name, rec_name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].icount; j++) {
                        free(recipes[i].ingredients[j].ing_name);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    for (size_t j = i; j < rcount - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    rcount--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *rec_name = line + 5;
            for (size_t i = 0; i < rcount; i++) {
                if (strcmp(recipes[i].name, rec_name) == 0) {
                    printf("Recipe %s:\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].icount; j++) {
                        printf("- %s: %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < rcount; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < rcount; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].icount; j++) {
            free(recipes[i].ingredients[j].ing_name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
